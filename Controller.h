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


enum CameraId { INSIDE_CAM, OUTSIDE_CAM, SIDE_CAM, TOP_CAM, CAM_COUNT };


class Controller : public irr::IEventReceiver
{
  public:
    Controller();
    void createScene();

    int run();

    void drawAxe();
    void drawBounds();

    void switchToCam(CameraId id);

    virtual bool OnEvent(const irr::SEvent& event);

    void setFrustum();

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

#endif

