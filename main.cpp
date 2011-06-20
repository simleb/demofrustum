/* Copyright (c) 2010 Simon Leblanc, for the Couzin Lab @ Princeton University

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

*/

#include <irrlicht.h>


#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif


enum CameraId { INSIDE_CAM, OUTSIDE_CAM, TOP_CAM, SIDE_CAM, FPS_CAM, MAYA_CAM, CAM_COUNT };


class Controller : public irr::IEventReceiver
{
  public:
    Controller() : m_device(0)
    {
        m_device = irr::createDevice(irr::video::EDT_NULL);
        const irr::s32 n = m_device->getVideoModeList()->getVideoModeCount();
        const irr::core::dimension2du size  = m_device->getVideoModeList()->getVideoModeResolution(n-1);
        const irr::s32 depth = m_device->getVideoModeList()->getVideoModeDepth(n-1);
        m_device->drop();

        m_device = irr::createDevice(irr::video::EDT_OPENGL, size, depth, true, false, false, 0);
        m_device->getCursorControl()->setVisible(false);
        m_device->setEventReceiver(this);

        m_driver = m_device->getVideoDriver();
        m_smgr = m_device->getSceneManager();

        createScene();

        m_smgr->setActiveCamera(m_cam[INSIDE_CAM]);
    }

    void createScene()
    {
        m_frustum = m_smgr->addEmptySceneNode();
        m_cam[INSIDE_CAM] = m_smgr->addCameraSceneNode(m_frustum);
        m_cam[INSIDE_CAM]->setNearValue(1);
        m_cam[INSIDE_CAM]->setFarValue(10);
        m_cam[INSIDE_CAM]->setFOV(irr::core::PI / 4.0);
        irr::scene::ISceneNode* cube = m_smgr->addCubeSceneNode(1, m_frustum, -1,
                                                              irr::core::vector3df(0,0,5),
                                                              irr::core::vector3df(-36,28,0));
        cube->setMaterialTexture(0, m_driver->getTexture("media/cube.jpg"));
        cube->setMaterialFlag(irr::video::EMF_LIGHTING, false);

        m_cam[OUTSIDE_CAM] = m_smgr->addCameraSceneNode(0, irr::core::vector3df(-8,3,-4), irr::core::vector3df(0,0,5));

        m_cam[TOP_CAM] = m_smgr->addCameraSceneNode(0, irr::core::vector3df(0,10,5), irr::core::vector3df(0,0,5));
        m_cam[TOP_CAM]->setUpVector(irr::core::vector3df(1,0,0));

        m_cam[SIDE_CAM] = m_smgr->addCameraSceneNode(0, irr::core::vector3df(-10,0,5), irr::core::vector3df(0,0,5));

        m_cam[FPS_CAM] = m_smgr->addCameraSceneNodeFPS(0, 100, 0.01);

        m_cam[MAYA_CAM] = m_smgr->addCameraSceneNodeMaya(0);
    }

    int run()
    {
        while (m_device->run())
        {
            m_driver->beginScene(true, true, 0);

            if (m_smgr->getActiveCamera() != m_cam[INSIDE_CAM])
            {
                const irr::scene::SViewFrustum* frustum = m_cam[INSIDE_CAM]->getViewFrustum();
                irr::video::SMaterial mat;
                mat.setFlag(irr::video::EMF_LIGHTING, false);
                m_driver->setMaterial(mat);
                m_driver->setTransform(irr::video::ETS_WORLD, irr::core::IdentityMatrix);

                m_driver->draw3DLine(irr::core::vector3df(0,0,0), frustum->getFarLeftDown());
                m_driver->draw3DLine(irr::core::vector3df(0,0,0), frustum->getFarLeftUp());
                m_driver->draw3DLine(irr::core::vector3df(0,0,0), frustum->getFarRightUp());
                m_driver->draw3DLine(irr::core::vector3df(0,0,0), frustum->getFarRightDown());

                m_driver->draw3DLine(frustum->getFarLeftDown(), frustum->getFarLeftUp());
                m_driver->draw3DLine(frustum->getFarLeftUp(), frustum->getFarRightUp());
                m_driver->draw3DLine(frustum->getFarRightUp(), frustum->getFarRightDown());
                m_driver->draw3DLine(frustum->getFarRightDown(), frustum->getFarLeftDown());

                irr::core::vector3df nlu;
                frustum->planes[irr::scene::SViewFrustum::VF_NEAR_PLANE].getIntersectionWithPlanes(
                    frustum->planes[irr::scene::SViewFrustum::VF_TOP_PLANE],
                    frustum->planes[irr::scene::SViewFrustum::VF_LEFT_PLANE], nlu);
                irr::core::vector3df nru;
                frustum->planes[irr::scene::SViewFrustum::VF_NEAR_PLANE].getIntersectionWithPlanes(
                    frustum->planes[irr::scene::SViewFrustum::VF_TOP_PLANE],
                    frustum->planes[irr::scene::SViewFrustum::VF_RIGHT_PLANE], nru);
                irr::core::vector3df nld;
                frustum->planes[irr::scene::SViewFrustum::VF_NEAR_PLANE].getIntersectionWithPlanes(
                    frustum->planes[irr::scene::SViewFrustum::VF_BOTTOM_PLANE],
                    frustum->planes[irr::scene::SViewFrustum::VF_LEFT_PLANE], nld);
                irr::core::vector3df nrd;
                frustum->planes[irr::scene::SViewFrustum::VF_NEAR_PLANE].getIntersectionWithPlanes(
                    frustum->planes[irr::scene::SViewFrustum::VF_BOTTOM_PLANE],
                    frustum->planes[irr::scene::SViewFrustum::VF_RIGHT_PLANE], nrd);

                m_driver->draw3DLine(nld, nlu);
                m_driver->draw3DLine(nlu, nru);
                m_driver->draw3DLine(nru, nrd);
                m_driver->draw3DLine(nrd, nld);
            }

            m_smgr->drawAll();

            m_driver->endScene();

            m_device->yield();
        }

        m_device->drop();

        return 0;
    }

    virtual bool OnEvent(const irr::SEvent& event)
    {
        if (event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)
        {
            if (event.KeyInput.Key == irr::KEY_KEY_1) // Inside cam
            {
                m_smgr->setActiveCamera(m_cam[INSIDE_CAM]);
                return true;
            }
            if (event.KeyInput.Key == irr::KEY_KEY_2) // Outide cam
            {
                m_smgr->setActiveCamera(m_cam[OUTSIDE_CAM]);
                return true;
            }
            if (event.KeyInput.Key == irr::KEY_KEY_3) // Top cam
            {
                m_smgr->setActiveCamera(m_cam[TOP_CAM]);
                return true;
            }
            if (event.KeyInput.Key == irr::KEY_KEY_4) // Side cam
            {
                m_smgr->setActiveCamera(m_cam[SIDE_CAM]);
                return true;
            }
            if (event.KeyInput.Key == irr::KEY_KEY_5) // FPS cam
            {
                m_smgr->setActiveCamera(m_cam[FPS_CAM]);
                return true;
            }
            if (event.KeyInput.Key == irr::KEY_KEY_6) // Maya cam
            {
                m_smgr->setActiveCamera(m_cam[MAYA_CAM]);
                return true;
            }
            if (event.KeyInput.Key == irr::KEY_ESCAPE) // Quit
            {
                m_device->closeDevice();
                return true;
            }
        }
        return false;
    }

  protected:
    irr::IrrlichtDevice* m_device;
    irr::video::IVideoDriver* m_driver;
    irr::scene::ISceneManager* m_smgr;
    CameraId m_camid;
    irr::scene::ICameraSceneNode* m_cam[CAM_COUNT];
    irr::scene::ISceneNode* m_frustum;
};


int main()
{
    Controller controller;

	return controller.run();
}

