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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <irrlicht.h>


enum Mode { TARGET, FRUSTUM, MODE_COUNT };
enum Camera { INSIDE, OUTSIDE, SIDE, TOP, CAM_COUNT };
enum Element { CUBE, AXIS, BOUNDS, NEAR, FAR, SCREEN, HELP, OBJECT_COUNT };


class Controller : public irr::IEventReceiver
{
  public:
    Controller();
    void makeDevice();
    void makeScene();

    void initScene();
    void drawScene();

    void changeCamera(Camera id);
    void updateFrustum();

    int run();

    virtual bool OnEvent(const irr::SEvent& event);

  protected:
    bool m_show[OBJECT_COUNT];
    irr::u32 m_mode;

    irr::scene::ICameraSceneNode* m_inside_cam;
    irr::f32 m_znear;
    irr::f32 m_zfar;

    irr::scene::ICameraSceneNode* m_cam;
    irr::core::vector3df m_position[CAM_COUNT];

    irr::IrrlichtDevice* m_device;
    irr::video::IVideoDriver* m_driver;
    irr::scene::ISceneManager* m_smgr;

    irr::scene::ISceneNode* m_cube;
    irr::scene::ISceneNode* m_near;
    irr::scene::ISceneNode* m_far;
    irr::scene::ISceneNode* m_screen;
    irr::core::dimension2df m_screen_size;
    irr::gui::IGUIFont* m_font;
};

#endif

