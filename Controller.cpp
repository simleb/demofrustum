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


#include "Controller.h"

#ifdef _IRR_OSX_PLATFORM_
#include "CoreFoundation/CoreFoundation.h"
#endif


Controller::Controller() : m_mode(TARGET), m_znear(2), m_zfar(10)
{
    m_show[  CUBE] = true;
    m_show[  AXIS] = false;
    m_show[BOUNDS] = true;
    m_show[  NEAR] = false;
    m_show[   FAR] = false;
    m_show[SCREEN] = false;
    m_show[  HELP] = true;

    makeDevice();
    makeScene();
    initScene();
}


void Controller::makeDevice()
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

    const irr::f32 screen_width = 0.5 * (m_znear + m_zfar);
    m_screen_size.set(screen_width, screen_width * size.Height / size.Width);
}


void Controller::makeScene()
{
    m_smgr->setAmbientLight(irr::video::SColor(255, 224, 224, 224));

    m_inside_cam = m_smgr->addCameraSceneNode();
    m_cam = m_smgr->addCameraSceneNode();

    const irr::core::vector3df cube_position(0, 0, 0.5 * (m_zfar - m_znear));
    m_cube = m_smgr->addCubeSceneNode(1, 0, -1, cube_position, irr::core::vector3df(-36,28,0));
#ifdef _IRR_OSX_PLATFORM_
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
    char path[PATH_MAX];
    CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8*)path, PATH_MAX);
    CFRelease(resourcesURL);
    const irr::core::stringc resources(path);
    irr::video::ITexture* texture = m_driver->getTexture(resources + "/cube.jpg");
#else
    irr::video::ITexture* texture = m_driver->getTexture("cube.jpg");
#endif
    if (!texture) texture = m_driver->getTexture("media/cube.jpg");
    if (!texture) texture = m_driver->getTexture("../media/cube.jpg");
    if (!texture) texture = m_driver->getTexture("../../media/cube.jpg");
    m_cube->setMaterialTexture(0, texture);

    const irr::scene::IGeometryCreator* geo = m_smgr->getGeometryCreator();
    const irr::scene::IMeshManipulator* manip = m_smgr->getMeshManipulator();
    irr::scene::IMesh* mesh = geo->createPlaneMesh(m_screen_size, irr::core::dimension2du(1,1), 0, irr::core::dimension2df(1,1));
    manip->setVertexColorAlpha(mesh, 30);
    manip->transform(mesh, irr::core::matrix4().buildRotateFromTo(irr::core::vector3df(0,1,0), irr::core::vector3df(0,0,1)));
    m_screen = m_smgr->addMeshSceneNode(mesh, 0, -1, irr::core::vector3df(0, 0, 0.5 * m_zfar));
    m_screen->setMaterialType(irr::video::EMT_TRANSPARENT_VERTEX_ALPHA);
    m_screen->getMaterial(0).AmbientColor.set(255, 128, 128, 0);
    m_screen->getMaterial(0).BackfaceCulling = false;
    m_screen->setVisible(false);

    m_near = m_screen->clone();
    m_near->getMaterial(0).AmbientColor.set(255, 0, 0, 192);

    m_far = m_screen->clone();
    m_far->getMaterial(0).AmbientColor.set(255, 192, 0, 0);

    irr::gui::IGUIEnvironment* gui = m_device->getGUIEnvironment();
#ifdef _IRR_OSX_PLATFORM_
    m_font = gui->getFont(resources + "/fontcourier.bmp");
#else
    m_font = gui->getFont("fontcourier.bmp");
#endif
    if (!m_font) m_font = gui->getFont("media/fontcourier.bmp");
    if (!m_font) m_font = gui->getFont("../media/fontcourier.bmp");
    if (!m_font) m_font = gui->getFont("../../media/fontcourier.bmp");
    if (!m_font) m_font = gui->getBuiltInFont();
}


void Controller::initScene()
{
    m_position[OUTSIDE] = irr::core::vector3df(-0.8, 0.3, -0.4) * m_zfar;
    m_position[TOP] = irr::core::vector3df(0, 1 , 0.5) * m_zfar;
    m_position[SIDE] = irr::core::vector3df(-1, 0, 0.5) * m_zfar;

    m_inside_cam->setPosition(irr::core::vector3df(0, 0, 0));
    m_inside_cam->setTarget(m_screen->getPosition());

    m_cam->removeAnimators();
    if (m_smgr->getActiveCamera() == m_inside_cam)
        m_cam->setPosition(irr::core::vector3df(0, 0, 0));
    m_cam->setTarget(m_screen->getPosition());

    m_cube->setPosition(irr::core::vector3df(0, 0, 0.5 * (m_zfar - m_znear)));
}


int Controller::run()
{
    while (m_device->run())
    {
        if (m_cam->getPosition() == m_inside_cam->getPosition())
            m_smgr->setActiveCamera(m_inside_cam);
        else
            m_smgr->setActiveCamera(m_cam);

        m_driver->beginScene(true, true, 0);

        drawScene();
        m_inside_cam->render();
        m_smgr->drawAll();

        m_driver->endScene();

        m_device->yield();
    }

    m_device->drop();

    return 0;
}


void Controller::drawScene()
{
    const irr::core::vector3df cam = m_inside_cam->getPosition();
    const irr::core::vector3df screen = m_screen->getPosition();
    const irr::f32 zscreen = screen.Z - cam.Z;
    const irr::f32 q = m_znear / zscreen;
    const irr::f32 r = m_zfar / zscreen;
    const irr::f32 s = m_znear / m_zfar;
    const irr::f32 t = zscreen / m_zfar;
    const irr::scene::SViewFrustum* frustum = m_inside_cam->getViewFrustum();
    const irr::core::vector3df fld = frustum->getFarLeftDown();
    const irr::core::vector3df flu = frustum->getFarLeftUp();
    const irr::core::vector3df fru = frustum->getFarRightUp();
    const irr::core::vector3df frd = frustum->getFarRightDown();
    const irr::core::vector3df fc = 0.25 * (fld + flu + fru + frd);

    updateFrustum();

    m_driver->setTransform(irr::video::ETS_WORLD, irr::core::IdentityMatrix);

    m_driver->setMaterial(irr::video::SMaterial());

    if (m_show[AXIS] && m_smgr->getActiveCamera() != m_inside_cam)
        m_driver->draw3DLine(m_inside_cam->getPosition(), m_inside_cam->getTarget());

    if (m_show[BOUNDS] && m_smgr->getActiveCamera() != m_inside_cam)
    {
        m_driver->draw3DLine(cam, fld);
        m_driver->draw3DLine(cam, flu);
        m_driver->draw3DLine(cam, fru);
        m_driver->draw3DLine(cam, frd);

        m_driver->draw3DLine(fld, flu);
        m_driver->draw3DLine(flu, fru);
        m_driver->draw3DLine(fru, frd);
        m_driver->draw3DLine(frd, fld);

        m_driver->draw3DLine(s * (fld - cam) + cam, s * (flu - cam) + cam);
        m_driver->draw3DLine(s * (flu - cam) + cam, s * (fru - cam) + cam);
        m_driver->draw3DLine(s * (fru - cam) + cam, s * (frd - cam) + cam);
        m_driver->draw3DLine(s * (frd - cam) + cam, s * (fld - cam) + cam);

        if (m_show[SCREEN])
        {
            m_driver->draw3DLine(t * (fld - cam) + cam, t * (flu - cam) + cam);
            m_driver->draw3DLine(t * (flu - cam) + cam, t * (fru - cam) + cam);
            m_driver->draw3DLine(t * (fru - cam) + cam, t * (frd - cam) + cam);
            m_driver->draw3DLine(t * (frd - cam) + cam, t * (fld - cam) + cam);
        }
    }

    if (m_show[NEAR])
    {
        m_near->setPosition(s * (fc - cam) + cam);
        m_near->setScale(irr::core::vector3df(q));
        if (m_mode == TARGET)
        {
            const irr::core::vector3df v = (screen-cam).getSphericalCoordinateAngles();
            m_near->setRotation(irr::core::vector3df(90+v.X, 90-(v.Y==0?+90:v.Y), 0));
        }
        else
        {
            m_near->setRotation(irr::core::vector3df(0));
        }
    }

    if (m_show[FAR])
    {
        m_far->setPosition(fc);
        m_far->setScale(irr::core::vector3df(r));
        if (m_mode == TARGET)
        {
            const irr::core::vector3df v = (screen-cam).getSphericalCoordinateAngles();
            m_far->setRotation(irr::core::vector3df(90+v.X, 90-(v.Y==0?+90:v.Y), 0));
        }
        else
        {
            m_far->setRotation(irr::core::vector3df(0));
        }
    }

    if (m_show[HELP])
    {
        irr::core::stringw help = "Help:\n\n";
        help += "H : Toggle help\n";
        help += "C : Toggle cube (";
        help += (m_show[CUBE] ? "On)\n" : "Off)\n");
        help += "A : Toggle axis (";
        help += (m_show[AXIS] ? "On)\n" : "Off)\n");
        help += "B : Toggle bounds (";
        help += (m_show[BOUNDS] ? "On)\n" : "Off)\n");
        help += "N : Toggle near plane coloring (";
        help += (m_show[NEAR] ? "On)\n" : "Off)\n");
        help += "F : Toggle far plane coloring (";
        help += (m_show[FAR] ? "On)\n" : "Off)\n");
        help += "S : Toggle screen plane coloring (";
        help += (m_show[SCREEN] ? "On)\n" : "Off)\n");
        help += "R : Reset\n";
        help += "Esc : Quit\n\n\n";
        help += "Views:\n\n";
        help += "1 : Inside view";
        help += (m_smgr->getActiveCamera() == m_inside_cam ? " (Current)\n" : "\n");
        help += "2 : Outside view";
        help += (m_cam->getPosition() == m_position[OUTSIDE] ? " (Current)\n" : "\n");
        help += "3 : Side view";
        help += (m_cam->getPosition() == m_position[SIDE] ? " (Current)\n" : "\n");
        help += "4 : Top view";
        help += (m_cam->getPosition() == m_position[TOP] ? " (Current)\n\n\n" : "\n\n\n");
        help += "Camera:\n\n";
        help += "Tab: Toggle mode";
        help += (m_mode == TARGET ? " (Target)\n" : " (Frustum)\n");
        help += "Arrows: Move camera parallel to screen plane\n";
        help += "P: Move camera towards screen plane\n";
        help += "M: Move camera away from screen plane\n\n\n";
        help += "Cube:\n\n";
        help += "Shift + Arrows: Move cube parallel to screen plane\n";
        help += "Shift + P: Move cube towards screen plane\n";
        help += "Shift + M: Move cube away from screen plane";
        const irr::core::dimension2du dim = m_font->getDimension(help.c_str());
        m_font->draw(help, irr::core::recti(20, 20, 20 + dim.Width, 20 + dim.Height), irr::video::SColor(255,255,255,255));
    }
}


void Controller::changeCamera(Camera id)
{
    irr::scene::ISceneNodeAnimator* anim;
    const irr::core::vector3df new_position = (id == INSIDE ? m_inside_cam->getPosition() : m_position[id]);
    anim = m_smgr->createFlyStraightAnimator(m_cam->getPosition(), new_position, 300);
    m_cam->addAnimator(anim);
    anim->drop();
}


bool Controller::OnEvent(const irr::SEvent& event)
{
    if (event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)
    {
        switch (event.KeyInput.Key)
        {
          case irr::KEY_KEY_1: // Inside cam
            changeCamera(INSIDE);
            return true;
          case irr::KEY_KEY_2: // Outide cam
            changeCamera(OUTSIDE);
            return true;
          case irr::KEY_KEY_3: // Side cam
            changeCamera(SIDE);
            return true;
          case irr::KEY_KEY_4: // Top cam
            changeCamera(TOP);
            return true;
          case irr::KEY_KEY_C: // Toggle cube
            m_show[CUBE] = !m_show[CUBE];
            m_cube->setVisible(m_show[CUBE]);
            return true;
          case irr::KEY_KEY_A: // Toggle axe
            m_show[AXIS] = !m_show[AXIS];
            return true;
          case irr::KEY_KEY_B: // Toggle bounds
            m_show[BOUNDS] = !m_show[BOUNDS];
            return true;
          case irr::KEY_KEY_N: // Toggle near plane coloring
            m_show[NEAR] = !m_show[NEAR];
            m_near->setVisible(m_show[NEAR]);
            return true;
          case irr::KEY_KEY_F: // Toggle far plane coloring
            m_show[FAR] = !m_show[FAR];
            m_far->setVisible(m_show[FAR]);
            return true;
          case irr::KEY_KEY_S: // Toggle screen plane coloring
            m_show[SCREEN] = !m_show[SCREEN];
            m_screen->setVisible(m_show[SCREEN]);
            return true;
          case irr::KEY_KEY_H: // Toggle help
            m_show[HELP] = !m_show[HELP];
            return true;
          case irr::KEY_KEY_R: // Reset position
            initScene();
            return true;
          case irr::KEY_TAB: // Switch camera behavior mode
            m_mode = (m_mode + 1) % MODE_COUNT;
            return true;
          case irr::KEY_ESCAPE: // Quit
            m_device->closeDevice();
            return true;
          default:
            return false;
        }
    }
    if (event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown && !event.KeyInput.Shift) // Move inside camera
    {
        const irr::f32 step = 0.05;
        const irr::core::vector3df position = m_inside_cam->getPosition();
        switch (event.KeyInput.Key)
        {
          case irr::KEY_LEFT:
            m_inside_cam->setPosition(position - irr::core::vector3df(step,0,0));
            break;
          case irr::KEY_RIGHT:
            m_inside_cam->setPosition(position + irr::core::vector3df(step,0,0));
            break;
          case irr::KEY_UP:
            m_inside_cam->setPosition(position + irr::core::vector3df(0,step,0));
            break;
          case irr::KEY_DOWN:
            m_inside_cam->setPosition(position - irr::core::vector3df(0,step,0));
            break;
          case irr::KEY_KEY_P:
            m_inside_cam->setPosition(position + irr::core::vector3df(0,0,step));
            break;
          case irr::KEY_KEY_M:
            m_inside_cam->setPosition(position - irr::core::vector3df(0,0,step));
            break;
          default:
            return false;
        }
        m_cam->removeAnimators();
        m_cam->setPosition(m_smgr->getActiveCamera()->getPosition());
        return true;
    }
    if (event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown && event.KeyInput.Shift) // Move cube
    {
        const irr::f32 step = 0.1;
        switch (event.KeyInput.Key)
        {
          case irr::KEY_LEFT:
            m_cube->setPosition(m_cube->getPosition() - irr::core::vector3df(step,0,0));
            return true;
          case irr::KEY_RIGHT:
            m_cube->setPosition(m_cube->getPosition() + irr::core::vector3df(step,0,0));
            return true;
          case irr::KEY_UP:
            m_cube->setPosition(m_cube->getPosition() + irr::core::vector3df(0,step,0));
            return true;
          case irr::KEY_DOWN:
            m_cube->setPosition(m_cube->getPosition() - irr::core::vector3df(0,step,0));
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
    return false;
}


void Controller::updateFrustum()
{
    const irr::core::vector3df cam = m_inside_cam->getPosition();
    const irr::core::vector3df screen = m_screen->getPosition();
    const irr::f32 zscreen = screen.Z - cam.Z;
    const irr::f32 q = m_znear / zscreen;
    irr::core::matrix4 proj;

    if (m_mode == TARGET)
    {
        const irr::f32 width = q * m_screen_size.Width;
        const irr::f32 height = q * m_screen_size.Height;

        proj[0] = (2.0 * m_znear) / width;
        proj[5] = (2.0 * m_znear) / height;
        proj[10] = (m_zfar + m_znear) / (m_zfar - m_znear);
        proj[11] = 1.0;
        proj[14] = -(2.0 * m_zfar * m_znear) / (m_zfar - m_znear);
        proj[15] = 0.0;

        m_inside_cam->setTarget(screen);
    }
    if (m_mode == FRUSTUM)
    {
        const irr::f32 xmin = q * (cam.X - screen.X - 0.5 * m_screen_size.Width);
        const irr::f32 xmax = q * (cam.X - screen.X + 0.5 * m_screen_size.Width);
        const irr::f32 ymin = q * (cam.Y - screen.Y - 0.5 * m_screen_size.Height);
        const irr::f32 ymax = q * (cam.Y - screen.Y + 0.5 * m_screen_size.Height);

        proj[0] = (2.0 * m_znear) / (xmax - xmin);
        proj[5] = (2.0 * m_znear) / (ymax - ymin);
        proj[8] = (xmax + xmin) / (xmax - xmin);
        proj[9] = (ymax + ymin) / (ymax - ymin);
        proj[10] = (m_zfar + m_znear) / (m_zfar - m_znear);
        proj[11] = 1.0;
        proj[14] = -(2.0 * m_zfar * m_znear) / (m_zfar - m_znear);
        proj[15] = 0.0;

        m_inside_cam->setTarget(cam + irr::core::vector3df(0, 0, m_zfar));
    }
    m_inside_cam->setProjectionMatrix(proj);
}

