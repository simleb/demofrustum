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
    Controller() : m_axe(false), m_bounds(true), m_znear(2), m_zfar(10), m_device(0), m_near_center(0,0,m_znear)
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

        const irr::f32 scale_fov = m_znear;
        m_near_size.set(scale_fov, scale_fov * size.Height / size.Width);

        createScene();
    }


    void createScene()
    {
        m_inside_cam = m_smgr->addCameraSceneNode(0, irr::core::vector3df(0,0,0), irr::core::vector3df(0,0,5));

        const irr::core::vector3df cube_position(0, 0, 0.5 * (m_zfar - m_znear));
        m_cube = m_smgr->addCubeSceneNode(1, 0, -1, cube_position, irr::core::vector3df(-36,28,0));
        m_cube->setMaterialTexture(0, m_driver->getTexture("media/cube.jpg"));
        m_cube->setMaterialFlag(irr::video::EMF_LIGHTING, false);

        setFrustum();
        const irr::scene::IGeometryCreator* geo = m_smgr->getGeometryCreator();
        irr::scene::IMesh* near_mesh = geo->createPlaneMesh(m_near_size, irr::core::dimension2du(1,1), 0,
                                                            irr::core::dimension2df(1,1));
        m_smgr->getMeshManipulator()->setVertexColorAlpha(near_mesh, 30);
        m_near = m_smgr->addMeshSceneNode(near_mesh, 0, -1, m_near_center, irr::core::vector3df(-90,0,0));
        m_near->setMaterialType(irr::video::EMT_TRANSPARENT_VERTEX_ALPHA);
        m_near->getMaterial(0).EmissiveColor.set(255, 0, 0, 255);
        m_near->getMaterial(0).BackfaceCulling = false;
        m_near->setVisible(false);

        const irr::f32 r = m_zfar / m_znear;
        irr::scene::IMesh* far_mesh = geo->createPlaneMesh(m_near_size * r, irr::core::dimension2du(1,1), 0,
                                                           irr::core::dimension2df(1,1));
        m_smgr->getMeshManipulator()->setVertexColorAlpha(far_mesh, 30);
        m_far = m_smgr->addMeshSceneNode(far_mesh, 0, -1, m_near_center * r, irr::core::vector3df(-90,0,0));
        m_far->setMaterialType(irr::video::EMT_TRANSPARENT_VERTEX_ALPHA);
        m_far->getMaterial(0).EmissiveColor.set(255, 255, 0, 0);
        m_far->getMaterial(0).BackfaceCulling = false;
        m_far->setVisible(false);

        m_position[OUTSIDE_CAM] = irr::core::vector3df(-8,3,-4);
        m_position[TOP_CAM] = irr::core::vector3df(0,10,5);
        m_position[SIDE_CAM] = irr::core::vector3df(-10,0,5);

        m_cam = m_smgr->addCameraSceneNode();
        m_cam->setTarget(cube_position);
    }


    int run()
    {
        while (m_device->run())
        {
            if (m_cam->getPosition() == m_inside_cam->getPosition())
                m_smgr->setActiveCamera(m_inside_cam);
            else
                m_smgr->setActiveCamera(m_cam);

            setFrustum();

            const irr::core::vector3df pos(-m_near_center.X,-m_near_center.Y,m_near_center.Z);
            m_near->setPosition(pos);
            m_far->setPosition(pos * m_zfar / m_znear);

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
        m_driver->draw3DLine(irr::core::vector3df(0,0,0), irr::core::vector3df(0,0,m_zfar));
    }


    void drawBounds()
    {
        m_driver->draw3DLine(irr::core::vector3df(0,0,0), m_frustum.getFarLeftDown());
        m_driver->draw3DLine(irr::core::vector3df(0,0,0), m_frustum.getFarLeftUp());
        m_driver->draw3DLine(irr::core::vector3df(0,0,0), m_frustum.getFarRightUp());
        m_driver->draw3DLine(irr::core::vector3df(0,0,0), m_frustum.getFarRightDown());

        m_driver->draw3DLine(m_frustum.getFarLeftDown(),  m_frustum.getFarLeftUp());
        m_driver->draw3DLine(m_frustum.getFarLeftUp(),    m_frustum.getFarRightUp());
        m_driver->draw3DLine(m_frustum.getFarRightUp(),   m_frustum.getFarRightDown());
        m_driver->draw3DLine(m_frustum.getFarRightDown(), m_frustum.getFarLeftDown());

        const irr::f32 s = m_znear / m_zfar;
        m_driver->draw3DLine(s * m_frustum.getFarLeftDown(),  s * m_frustum.getFarLeftUp());
        m_driver->draw3DLine(s * m_frustum.getFarLeftUp(),    s * m_frustum.getFarRightUp());
        m_driver->draw3DLine(s * m_frustum.getFarRightUp(),   s * m_frustum.getFarRightDown());
        m_driver->draw3DLine(s * m_frustum.getFarRightDown(), s * m_frustum.getFarLeftDown());
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
            switch (event.KeyInput.Key)
            {
              case irr::KEY_KEY_1: // Inside cam
                switchToCam(INSIDE_CAM);
                return true;
              case irr::KEY_KEY_2: // Outide cam
                switchToCam(OUTSIDE_CAM);
                return true;
              case irr::KEY_KEY_3: // Side cam
                switchToCam(SIDE_CAM);
                return true;
              case irr::KEY_KEY_4: // Top cam
                switchToCam(TOP_CAM);
                return true;
              case irr::KEY_KEY_N: // Toggle near plan coloring
                m_near->setVisible(!m_near->isVisible());
                return true;
              case irr::KEY_KEY_F: // Toggle far plan coloring
                m_far->setVisible(!m_far->isVisible());
                return true;
              case irr::KEY_KEY_B: // Toggle bounds
                m_bounds = !m_bounds;
                return true;
              case irr::KEY_KEY_A: // Toggle axe
                m_axe = !m_axe;
                return true;
              case irr::KEY_KEY_R: // Reset position
                    m_near_center.set(0, 0, m_znear);
                    m_cube->setPosition(irr::core::vector3df(0 , 0, 0.5 * (m_zfar - m_znear)));
                return true;
              case irr::KEY_ESCAPE: // Quit
                m_device->closeDevice();
                return true;
              default:
                return false;
            }
        }
        if (event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown && !event.KeyInput.Shift) // Move cube
        {
            const irr::f32 step = 0.2;
            switch (event.KeyInput.Key)
            {
              case irr::KEY_UP:
                m_cube->setPosition(m_cube->getPosition() + irr::core::vector3df(0,step,0));
                return true;
              case irr::KEY_DOWN:
                m_cube->setPosition(m_cube->getPosition() - irr::core::vector3df(0,step,0));
                return true;
              case irr::KEY_LEFT:
                m_cube->setPosition(m_cube->getPosition() - irr::core::vector3df(step,0,0));
                return true;
              case irr::KEY_RIGHT:
                m_cube->setPosition(m_cube->getPosition() + irr::core::vector3df(step,0,0));
                return true;
              case irr::KEY_KEY_P:
                m_cube->setPosition(m_cube->getPosition() + irr::core::vector3df(0,0,step));
                return true;
              case irr::KEY_KEY_M:
                m_cube->setPosition(m_cube->getPosition() - irr::core::vector3df(0,0,step));
                return true;
              default:
                return false;
            }
        }
        if (event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown && event.KeyInput.Shift) // Move near plane
        {
            const irr::f32 step = 0.05;
            switch (event.KeyInput.Key)
            {
              case irr::KEY_UP:
                m_near_center -= irr::core::vector3df(0,step,0);
                return true;
              case irr::KEY_DOWN:
                m_near_center += irr::core::vector3df(0,step,0);
                return true;
              case irr::KEY_LEFT:
                m_near_center += irr::core::vector3df(step,0,0);
                return true;
              case irr::KEY_RIGHT:
                m_near_center -= irr::core::vector3df(step,0,0);
                return true;
              default:
                return false;
            }
        }
        return false;
    }


    void setFrustum()
    {
        const irr::f32 xmin = m_near_center.X - 0.5 * m_near_size.Width;
        const irr::f32 xmax = m_near_center.X + 0.5 * m_near_size.Width;
        const irr::f32 ymin = m_near_center.Y - 0.5 * m_near_size.Height;
        const irr::f32 ymax = m_near_center.Y + 0.5 * m_near_size.Height;

        irr::core::matrix4 proj;
        proj[0] = (2.0 * m_znear) / (xmax - xmin);
        proj[5] = (2.0 * m_znear) / (ymax - ymin);
        proj[8] = (xmax + xmin) / (xmax - xmin);
        proj[9] = (ymax + ymin) / (ymax - ymin);
        proj[10] = (m_zfar + m_znear) / (m_zfar - m_znear);
        proj[11] = 1.0;
        proj[14] = -(2.0 * m_zfar * m_znear) / (m_zfar - m_znear);
        proj[15] = 0.0;
        m_inside_cam->setProjectionMatrix(proj);

        m_frustum = irr::scene::SViewFrustum(*m_inside_cam->getViewFrustum());
        m_frustum.cameraPosition = m_inside_cam->getAbsolutePosition();
        irr::core::matrix4 m(irr::core::matrix4::EM4CONST_NOTHING);
        m.setbyproduct_nocheck(m_frustum.getTransform(irr::video::ETS_PROJECTION),
                               m_frustum.getTransform(irr::video::ETS_VIEW));
        m_frustum.setFrom(m);
    }


  protected:
    bool m_axe;
    bool m_bounds;
    irr::f32 m_znear;
    irr::f32 m_zfar;
    irr::IrrlichtDevice* m_device;
    irr::video::IVideoDriver* m_driver;
    irr::scene::ISceneManager* m_smgr;
    irr::scene::ICameraSceneNode* m_inside_cam;
    irr::scene::ICameraSceneNode* m_cam;
    irr::core::vector3df m_position[CAM_COUNT];
    irr::scene::ISceneNode* m_cube;
    irr::scene::SViewFrustum m_frustum;
    irr::scene::IMeshSceneNode* m_near;
    irr::scene::IMeshSceneNode* m_far;
    irr::core::vector3df m_near_center;
    irr::core::dimension2df m_near_size;
};


int main()
{
    Controller controller;

	return controller.run();
}

