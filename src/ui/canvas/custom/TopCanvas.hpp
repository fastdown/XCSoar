/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2021 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#ifndef XCSOAR_SCREEN_TOP_CANVAS_HPP
#define XCSOAR_SCREEN_TOP_CANVAS_HPP

#include "util/Compiler.h"

#ifdef USE_MEMORY_CANVAS
#include "ui/canvas/memory/PixelTraits.hpp"
#include "ui/canvas/memory/ActivePixelTraits.hpp"
#include "ui/canvas/memory/Buffer.hpp"
#include "ui/dim/Size.hpp"
#else
#include "ui/canvas/Canvas.hpp"
#endif

#ifdef ENABLE_OPENGL
#include "ui/opengl/Features.hpp"
#endif

#ifdef USE_EGL
#include "ui/egl/System.hpp"

#ifdef MESA_KMS
#include <drm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#endif
#endif

#ifdef USE_GLX
#include "ui/glx/System.hpp"

#define Font X11Font
#define Window X11Window
#define Display X11Display
#include <X11/X.h>
#undef Font
#undef Window
#undef Display
#undef Expose
#undef KeyPress
struct _XDisplay;
#endif

#ifdef DITHER
#include "../memory/Dither.hpp"
#endif

#include <cstdint>

#ifdef SOFTWARE_ROTATE_DISPLAY
enum class DisplayOrientation : uint8_t;
#endif

struct SDL_Surface;
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
class Canvas;
struct PixelSize;
struct PixelRect;

#if (defined(USE_FB) && !defined(KOBO)) || defined(USE_EGL)
/* defined if we need to initialise /dev/tty to graphics mode, see
   TopCanvas::InitialiseTTY() */
#define USE_TTY
#include "ui/linux/GraphicsTTY.hpp"
#endif

class TopCanvas
#ifndef USE_MEMORY_CANVAS
  : public Canvas
#endif
{
#ifdef USE_TTY
  const LinuxGraphicsTTY linux_graphics_tty;
#endif

#ifdef USE_EGL
#if defined(USE_X11) || defined(USE_WAYLAND)
#elif defined(MESA_KMS)
  struct gbm_device *native_display;
  struct gbm_surface *native_window;

  int dri_fd;

  struct gbm_bo *current_bo = nullptr;

  drmEventContext evctx;

  drmModeConnector *connector = nullptr;
  drmModeEncoder *encoder = nullptr;
  drmModeModeInfo mode;

  drmModeCrtc *saved_crtc = nullptr;
#endif

  EGLDisplay display;
#ifndef ANDROID
  EGLContext context;
#endif
  EGLSurface surface;
#endif

#ifdef USE_GLX
  _XDisplay *const x_display;
  GLXContext glx_context;
  GLXWindow glx_window;
#endif

#ifdef ENABLE_SDL
  SDL_Window *const window;

#ifdef USE_MEMORY_CANVAS
  SDL_Renderer *renderer;
  SDL_Texture *texture;
#endif
#endif

#ifdef USE_MEMORY_CANVAS

#ifdef GREYSCALE
  WritableImageBuffer<GreyscalePixelTraits> buffer;

#ifdef DITHER
  Dither dither;
#endif

#else /* !GREYSCALE */
  WritableImageBuffer<ActivePixelTraits> buffer;
#endif /* !GREYSCALE */
#endif /* USE_MEMORY_CANVAS */

#ifdef USE_FB
  int fd = -1;

  void *map = nullptr;
  unsigned map_pitch, map_bpp;

  uint32_t epd_update_marker;
#endif

#ifdef KOBO
  /**
   * Runtime flag that can be used to disable dithering at runtime for
   * some situations.
   */
  bool enable_dither = true;

  /**
   * some kobo Device don't need to wait eInk update complet before send new update cmd
   * this flag can be set true for don't wait eInk Update complete for faster responce time.
   */
  bool frame_sync = false;
#endif

public:
#ifdef ENABLE_SDL
  TopCanvas(SDL_Window *_window);
#elif defined(USE_GLX)
  TopCanvas(_XDisplay *x_display,
            X11Window x_window,
            GLXFBConfig *fb_cfg);
#elif defined(USE_X11) || defined(USE_WAYLAND)
  TopCanvas(EGLNativeDisplayType native_display,
            EGLNativeWindowType native_window) {
    CreateEGL(native_display, native_window);
  }
#elif defined(ANDROID) || defined(USE_VFB)
  TopCanvas(PixelSize new_size);
#else
  TopCanvas();
#endif

  ~TopCanvas() noexcept;

#ifdef USE_MEMORY_CANVAS
  gcc_pure
  PixelRect GetRect() const;
#endif

#if defined(USE_FB) || (defined(ENABLE_OPENGL) && (defined(USE_EGL) || defined(USE_GLX) || defined(ENABLE_SDL)))
  /**
   * Obtain the native (non-software-rotated) size of the OpenGL
   * drawable.
   */
  gcc_pure
  PixelSize GetNativeSize() const;
#endif

#if defined(USE_MEMORY_CANVAS) || defined(ENABLE_OPENGL)
  /**
   * Check if the screen has been resized.
   *
   * @param new_native_size the new screen size reported by the
   * windowing system library
   * @return true if the screen has been resized
   */
  bool CheckResize(PixelSize new_native_size);
#endif

#ifdef USE_FB
  /**
   * Ask the kernel for the frame buffer's current physical size.
   * This is used by CheckResize().
   */
  gcc_pure
  PixelSize GetPhysicalSize() const;

  /**
   * Check if the screen has been resized.
   *
   * @return true if the screen has been resized
   */
  bool CheckResize();
#endif

#ifdef ENABLE_OPENGL
  /**
   * Initialise the new OpenGL context.
   */
  void Resume();
#endif

#if defined(ENABLE_SDL) && defined(USE_MEMORY_CANVAS)
  void OnResize(PixelSize new_size);
#endif

#ifdef USE_MEMORY_CANVAS
  PixelSize GetSize() const {
    return PixelSize(buffer.width, buffer.height);
  }

  Canvas Lock();
  void Unlock();
#endif

  void Flip();

#ifdef KOBO
  /**
   * Wait until the screen update is complete.
   */
  void Wait();

  void SetEnableDither(bool _enable_dither) {
    enable_dither = _enable_dither;
  }
#endif

#ifdef SOFTWARE_ROTATE_DISPLAY
  void SetDisplayOrientation(DisplayOrientation orientation);
#endif

private:
#ifdef ENABLE_OPENGL
  void SetupViewport(PixelSize native_size);
#endif

#ifdef USE_EGL
  void CreateEGL(EGLNativeDisplayType native_display,
                 EGLNativeWindowType native_window);
#endif
};

#endif
