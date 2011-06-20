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


class Controller : public irr::IEventReceiver
{
  public:
    Controller(irr::IrrlichtDevice* device) : m_device(device), m_step(0)
    {
        m_device->setEventReceiver(this);
    }

    virtual bool OnEvent(const irr::SEvent& event)
    {
        if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key == irr::KEY_ESCAPE)
        {
            m_device->closeDevice();
            return true;
        }
        return false;
    }

  protected:
    irr::u32 m_step;
    irr::IrrlichtDevice* m_device;
};


int main()
{
    // Create a NULL device to detect available video modes
    irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_NULL);
    const irr::s32 n = device->getVideoModeList()->getVideoModeCount();

    // Screen size
    const irr::core::dimension2du size  = device->getVideoModeList()->getVideoModeResolution(n-1);

    // Screen depth
    const irr::s32 depth = device->getVideoModeList()->getVideoModeDepth(n-1);

    device->drop();

    device = irr::createDevice(irr::video::EDT_OPENGL, size, depth, true, false, false, 0);

    device->getCursorControl()->setVisible(false);

	if (!device) return 1;

    Controller controller(device);

    irr::video::IVideoDriver* driver = device->getVideoDriver();
    irr::scene::ISceneManager* smgr = device->getSceneManager();
    irr::gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

    irr::scene::ICameraSceneNode *cam = smgr->addCameraSceneNode();

	while(device->run())
	{
		// Render graphics
		driver->beginScene(true, true, irr::video::SColor(255,100,101,140));

		smgr->drawAll();
		guienv->drawAll();

		driver->endScene();

		// Sleep a tiny bit
		device->yield();
	}

	device->drop();

	return 0;
}

