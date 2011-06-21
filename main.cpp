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


enum CameraId { INSIDE_CAM, OUTSIDE_CAM, SIDE_CAM, TOP_CAM, CAM_COUNT };


class Controller : public irr::IEventReceiver
{
  public:
    Controller() : m_device(0), m_bounds(true), m_axe(true)
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
    }


    void createScene()
    {
        m_inside_cam = m_smgr->addCameraSceneNode(0, irr::core::vector3df(0,0,0), irr::core::vector3df(0,0,5));
        m_inside_cam->setNearValue(1);
        m_inside_cam->setFarValue(10);
        m_inside_cam->setFOV(irr::core::PI / 4.0);

        m_frustum_node = m_smgr->addEmptySceneNode();
        irr::scene::ISceneNode* cube = m_smgr->addCubeSceneNode(1, 0, -1,
                                                                irr::core::vector3df(0,0,5),
                                                                irr::core::vector3df(-36,28,0));
        cube->setMaterialTexture(0, m_driver->getTexture("media/cube.jpg"));
        cube->setMaterialFlag(irr::video::EMF_LIGHTING, false);

        irr::scene::IMesh* mesh = m_smgr->getGeometryCreator()->createPlaneMesh(irr::core::dimension2df(1.3255,0.8285),
                                                                                irr::core::dimension2du(1,1),
                                                                                0,
                                                                                irr::core::dimension2df(1,1));
        m_smgr->getMeshManipulator()->setVertexColorAlpha(mesh, 30);
        m_near = m_smgr->addMeshSceneNode(mesh, m_frustum_node, -1, irr::core::vector3df(0,0,1), irr::core::vector3df(-90,0,0));
        m_near->setMaterialType(irr::video::EMT_TRANSPARENT_VERTEX_ALPHA);
        m_near->getMaterial(0).EmissiveColor.set(255, 0, 0, 255);
        m_near->getMaterial(0).BackfaceCulling = false;
        m_near->setVisible(false);

        irr::scene::IMesh* mesh2 = m_smgr->getGeometryCreator()->createPlaneMesh(irr::core::dimension2df(13.255,8.285),
                                                                                 irr::core::dimension2du(1,1),
                                                                                 0,
                                                                                 irr::core::dimension2df(1,1));
        m_smgr->getMeshManipulator()->setVertexColorAlpha(mesh2, 30);
        m_far = m_smgr->addMeshSceneNode(mesh2, m_frustum_node, -1, irr::core::vector3df(0,0,10), irr::core::vector3df(-90,0,0));
        m_far->setMaterialType(irr::video::EMT_TRANSPARENT_VERTEX_ALPHA);
        m_far->getMaterial(0).EmissiveColor.set(255, 255, 0, 0);
        m_far->getMaterial(0).BackfaceCulling = false;
        m_far->setVisible(false);

        m_position[OUTSIDE_CAM] = irr::core::vector3df(-8,3,-4);
        m_position[TOP_CAM] = irr::core::vector3df(0,10,5);
        m_position[SIDE_CAM] = irr::core::vector3df(-10,0,5);

        m_cam = m_smgr->addCameraSceneNode();
        m_cam->setTarget(irr::core::vector3df(0,0,5));
    }


    int run()
    {
        while (m_device->run())
        {
            if (m_cam->getPosition() == m_inside_cam->getPosition())
            {
                m_smgr->setActiveCamera(m_inside_cam);
                m_frustum_node->setVisible(false);
            }
            else
            {
                m_smgr->setActiveCamera(m_cam);
                m_frustum_node->setVisible(true);
            }

            irr::video::SMaterial mat;
            mat.setFlag(irr::video::EMF_LIGHTING, false);
            m_driver->setMaterial(mat);
            m_driver->setTransform(irr::video::ETS_WORLD, irr::core::IdentityMatrix);

            m_driver->beginScene(true, true, 0);

            if (m_bounds)
                drawBounds();

            if (m_axe)
                drawAxe();

            m_smgr->drawAll();

            m_driver->endScene();

            m_device->yield();
        }

        m_device->drop();

        return 0;
    }


    void drawAxe()
    {
        m_driver->draw3DLine(irr::core::vector3df(0,0,0), irr::core::vector3df(0,0,10));
    }


    void drawBounds()
    {
        const irr::scene::SViewFrustum* frustum = m_inside_cam->getViewFrustum();

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


    void switchToCam(CameraId id)
    {
        m_smgr->setActiveCamera(m_cam);
        irr::scene::ISceneNodeAnimator* anim;
        anim = m_smgr->createFlyStraightAnimator(m_cam->getPosition(), m_position[id], 300);
        m_cam->addAnimator(anim);
        anim->drop();
    }


    virtual bool OnEvent(const irr::SEvent& event)
    {
        if (event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)
        {
            if (event.KeyInput.Key == irr::KEY_KEY_1) // Inside cam
            {
                switchToCam(INSIDE_CAM);
                return true;
            }
            if (event.KeyInput.Key == irr::KEY_KEY_2) // Outide cam
            {
                switchToCam(OUTSIDE_CAM);
                return true;
            }
            if (event.KeyInput.Key == irr::KEY_KEY_3) // Side cam
            {
                switchToCam(SIDE_CAM);
                return true;
            }
            if (event.KeyInput.Key == irr::KEY_KEY_4) // Top cam
            {
                switchToCam(TOP_CAM);
                return true;
            }
            if (event.KeyInput.Key == irr::KEY_KEY_N) // Toggle near plan coloring
            {
                m_near->setVisible(!m_near->isVisible());
                return true;
            }
            if (event.KeyInput.Key == irr::KEY_KEY_F) // Toggle far plan coloring
            {
                m_far->setVisible(!m_far->isVisible());
                return true;
            }
            if (event.KeyInput.Key == irr::KEY_KEY_B) // Toggle bounds
            {
                m_bounds = !m_bounds;
                return true;
            }
            if (event.KeyInput.Key == irr::KEY_KEY_A) // Toggle axe
            {
                m_axe = !m_axe;
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
    irr::scene::ICameraSceneNode* m_inside_cam;
    irr::scene::ICameraSceneNode* m_cam;
    irr::core::vector3df m_position[CAM_COUNT];
    irr::scene::ISceneNode* m_frustum_node;
    bool m_axe;
    bool m_bounds;
    irr::scene::IMeshSceneNode* m_near;
    irr::scene::IMeshSceneNode* m_far;
};


int main()
{
    Controller controller;

	return controller.run();
}

