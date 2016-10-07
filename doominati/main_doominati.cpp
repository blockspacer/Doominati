//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Program entry point.
//
//-----------------------------------------------------------------------------

#include "Code/Codedefs.hpp"
#include "Code/Native.hpp"
#include "Code/Process.hpp"
#include "Code/Program.hpp"
#include "Code/Thread.hpp"

#include "Core/Time.hpp"
#include "Core/Math.hpp"

#include "FS/Dir.hpp"

#include "GL/Particle.hpp"
#include "GL/Shader.hpp"
#include "GL/Window.hpp"

#include "Game/Input.hpp"

#include <GDCC/Core/Option.hpp>

#include <iostream>
#include <cstdlib>

#include "SDL.h"


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

//
// ParticleSystem
//
static DGE::GL::ParticleSystem ParticleSystem{1280/2, 720/2, 1000, "@Textures/particle2.png"};

//
// TestShader
//
static std::unique_ptr<DGE::GL::Shader> TestShader;

//
// TestShaderFragment
//
static char const *TestShaderFragment = R"(
   #version 120

   uniform int dge_ticks;
   uniform int dge_mseconds;
   uniform int dge_seconds;

   vec4 HSVToRGB(float h, float s, float v, float a)
   {
      h *= 360.0;

           if(h < 0)     h = 0;
      else if(h > 359.5) h = 359.5;

      float chroma = v * s;
      float hp = h / 60.0;
      float x = chroma * (1 - abs(mod(hp, 2) - 1));
      float r1 = 0, g1 = 0, b1 = 0;

           if(0 <= hp && hp < 1) r1 = chroma, g1 = x,      b1 = 0;
      else if(1 <= hp && hp < 2) r1 = x,      g1 = chroma, b1 = 0;
      else if(2 <= hp && hp < 3) r1 = 0,      g1 = chroma, b1 = x;
      else if(3 <= hp && hp < 4) r1 = 0,      g1 = x,      b1 = chroma;
      else if(4 <= hp && hp < 5) r1 = x,      g1 = 0,      b1 = chroma;
      else if(5 <= hp && hp < 6) r1 = chroma, g1 = 0,      b1 = x;

      float m = v - chroma;
      return vec4(r1 + m, g1 + m, b1 + m, a);
   }

   void main(void)
   {
      const float pi = 3.14159265;

      float t = float(dge_mseconds) / 32.0;
      vec2 uv = (gl_FragCoord.xy / vec2(640.0, 480.0)) * vec2(256.0, 224.0);

      float bx = uv.x + 0.5 * sin(t / 8.0);
      float by = uv.y + 0.5 * cos(t / 8.0);

      float v;

      v  = sin((-bx + t) / 32.0);
      v += cos((by + t) / 32.0);
      v += sin((bx + by + t) / 32.0);
      v += sin((sqrt(pow(bx + sin(t / 3.0), 2.0) + pow(by + cos(t / 2.0), 2.0) + 1.0) + t) / 128.0);

      gl_FragColor = HSVToRGB(sin(v * pi) * 0.5 + 0.5, 1.0, 1.0, 1.0);
   }
)";

//
// TestShaderVertex
//
static char const *TestShaderVertex = R"(
   #version 120

   void main(void)
   {
      gl_Position = ftransform();
   }
)";


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// DrawTest
//
// Draws test pattern.
//
static void DrawTest()
{
   auto window = DGE::GL::Window::Current;

   auto w = window->w;
   auto h = window->h;

   auto xo = w / 3.0f;
   auto yo = h / 3.0f;

   auto xp = w - xo;
   auto yp = h - yo;

   double seconds = DGE::Core::GetTicks<DGE::Core::Second<double>>();

   window->drawParticleSystem(ParticleSystem);

   window->drawColorSet(DGE::GL::Color::White);

   window->textureBind("@Textures/bigscreen.ppm");
   window->drawRectangle(303, 2, 603, 302);

   window->shaderSwap(TestShader.get());
   window->shaderUpdate();
   window->drawRectangle(2, 2, 302, 302);
   window->shaderDrop();

   window->drawColorSet(DGE::GL::Color::FromHSV(std::sin(seconds) * 0.5f + 0.5f, 1.0, 1.0));

   window->textureBind("@Textures/colors.ppm");
   window->drawRectangle(w - 102, h - 102, w - 2, h - 2);
   window->textureUnbind();

   window->drawRectangle(xp, yp, xo, yo, 0, true);

   window->drawColorSet(DGE::GL::Color::Red);

   window->drawLine(xp, yp, xo, yo);
   window->drawLine(xp, yo, xo, yp);

   window->drawColorSet(DGE::GL::Color::Green);

   double s = std::sin(seconds * 4.0) * 40.0;
   double c = std::cos(seconds * 4.0) * 40.0;

   window->drawLine(xp + s, yp + c, xo + s, yo + c);
   window->drawLine(xp + s, yo + c, xo + s, yp + c);

   window->textureBind("@Textures/colors.ppm");

   DGE::GL::Color drawc{DGE::GL::Color::Pink};
   for(int i = 0; i < 3; i++)
   {
      window->drawColorSet(drawc);
      drawc.interpolate(DGE::GL::Color::Blue, 1.0f / 3.0f);

      window->drawCircle(64 + (i * 128), 550, 64);
   }

   window->textureUnbind();

   window->drawColorSet(DGE::GL::Color::Pink);
   window->drawEllipse(0, 614, 900, h);

   window->drawColorSet(DGE::GL::Color::Green);
   window->drawEllipse(0, 614, 900, h, true);

   window->drawColorSet(DGE::GL::Color::Purple);
   window->drawTriangle(900 + 50, h - 100, 900, h, 900 + 100, h);

   window->drawColorSet(DGE::GL::Color::FromHSV(std::sin(seconds * 0.25f) * 0.5f + 0.5f, 1.0, 1.0));
   window->drawTriangle(900 + 50, h - 100, 900, h, 900 + 100, h, true);
}

//
// DrawDigit
//
static void DrawDigit(unsigned int dig, int xl, int yl, int xh, int yh)
{
   int ym = (yl + yh) / 2;

   static struct
   {
      bool seg0 : 1, seg1 : 1, seg2 : 1, seg3 : 1, seg4 : 1, seg5 : 1, seg6 : 1;
   }
   const digtab[10]
   {
      {1,1,1,0,1,1,1},
      {0,0,1,0,0,1,0},
      {1,0,1,1,1,0,1},
      {1,0,1,1,0,1,1},
      {0,1,1,1,0,1,0},
      {1,1,0,1,0,1,1},
      {1,1,0,1,1,1,1},
      {1,0,1,0,0,1,0},
      {1,1,1,1,1,1,1},
      {1,1,1,1,0,1,1},
   };

   auto &digit = digtab[dig];

   //  -0-
   // |   |
   // 1   2
   // |   |
   //  -3-
   // |   |
   // 4   5
   // |   |
   //  -6-

   auto window = DGE::GL::Window::Current;

   window->lineSmooth(true);
   window->lineWidth(2);
   if(digit.seg0) window->drawLine(xl, yh, xh, yh);
   if(digit.seg1) window->drawLine(xl, ym, xl, yh);
   if(digit.seg2) window->drawLine(xh, ym, xh, yh);
   if(digit.seg3) window->drawLine(xl, ym, xh, ym);
   if(digit.seg4) window->drawLine(xl, yl, xl, ym);
   if(digit.seg5) window->drawLine(xh, yl, xh, ym);
   if(digit.seg6) window->drawLine(xl, yl, xh, yl);
   window->lineSmooth(false);
   window->lineWidth(1);
}

//
// DrawFPS
//
static void DrawFPS()
{
   static double timeLast = 0;
   static double timeMean = 1/50.0;
   double timeThis, timePass;

   timeThis = DGE::Core::GetTicks<DGE::Core::Second<double>>();
   timePass = timeThis - timeLast;
   timeLast = timeThis;
   timeMean = (timeMean * 19 + timePass) / 20;

   unsigned int fps = std::round(1 / timeMean);

   DGE::GL::Window::Current->drawColorSet(DGE::GL::Color::White);

   int x = DGE::GL::Window::Current->w - 65;
   int y = 35;

   if(fps > 999) fps = 999;

   DrawDigit(fps / 100 % 10, x +  0, y, x + 15, y - 25);
   DrawDigit(fps /  10 % 10, x + 20, y, x + 35, y - 25);
   DrawDigit(fps /   1 % 10, x + 40, y, x + 55, y - 25);
}

//
// LoadCodedefs
//
static void LoadCodedefs(DGE::Code::Program *prog)
{
   DGE::Code::Loader loader;
   DGE::FS::Dir::ForFile("codedefs",
      std::bind(&DGE::Code::Loader::loadCodedefs, &loader, std::placeholders::_1));

   std::cerr << "Loaded " << loader.loadPASS << " codedefs.\n";

   if(loader.loadFAIL)
   {
      std::cerr << "Encountered " << loader.loadFAIL << " codedefs errors.\n";
      throw EXIT_FAILURE;
   }

   loader.gen(prog);
}

//
// Main
//
static int Main()
{
   if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
   {
      std::cerr << "SDL_Init: " << SDL_GetError() << '\n';
      throw EXIT_FAILURE;
   }

   std::atexit(SDL_Quit);

   DGE::GL::Window window{640, 480};
   DGE::GL::Window::Current = &window;
   TestShader.reset(new DGE::GL::Shader{TestShaderFragment, TestShaderVertex});

   DGE::Game::InputSource_Local input;

   DGE::FS::Dir::AddRoot("."); // HACK

   // Initialize scripting and call main.
   DGE::Code::NativeAdder::Finish();
   DGE::Code::Program prog;
   LoadCodedefs(&prog);
   DGE::Code::Process proc{&prog};

   if(auto func = prog.funcs.find("main"))
      proc.mainThread()->startTask(func, nullptr, 0);

   std::size_t timeLast = DGE::Core::GetTicks<DGE::Core::PlayTick<>>();
   std::size_t timeNext;

   for(;;)
   {
      std::size_t timeDelta;

      timeNext  = DGE::Core::GetTicks<DGE::Core::PlayTick<>>();
      timeDelta = timeNext - timeLast;
      timeLast  = timeNext;

      if(timeDelta)
      {
         while(timeDelta--)
         {
            // Playsim actions.
            for(;;)
            {
               DGE::GL::Particle *test = ParticleSystem.create();

               if(test == nullptr)
                  break;

               float const r[] = {
                  float((rand() % 1280) - (1280/2)),
                  float((rand() % 720 ) - (720/2)),
                  float(rand() % 30),
                  float((rand() % 255) - 128),
                  float((rand() % 255) - 128),
                  float((rand() % 255) - 128),
                  float((rand() % 255) - 128),
                  float((rand() % 255) - 128),
                  float(rand() % 360) / 360.0f
               };

               test->life           = r[2] + 10;
               test->oldposition.x  = test->position.x = -30.0f + r[0];
               test->oldposition.y  = test->position.y = -30.0f + r[1];
               test->velocity.x     = (1.0f / 4096) * r[3];
               test->velocity.y     = (1.0f / 4096) * r[4];
               test->acceleration.x = (1.0f / 16384) * r[5];
               test->acceleration.y = (1.0f / 16384) * r[6];
               test->scale.x = test->scale.y = (1.0f / 4096) * r[7] * 40;
               test->color       = DGE::GL::Color::FromHSV(r[8], 1, 1);
               test->colordest   = DGE::GL::Color::Zero;
               test->colordest.a = 0.0f;
               test->colorspeed  = 0.04f;
            }

            input.poll();
            proc.exec();
            ParticleSystem.update();
         }
      }
      else
         SDL_Delay(1);

      // Rendering actions.
      window.renderBegin();

      DrawTest();
      DrawFPS();

      window.renderEnd();
   }

   return EXIT_SUCCESS;
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

//
// main
//
int main(int argc, char **argv)
{
   auto &opts = GDCC::Core::GetOptions();

   // Don't want an output option.
   opts.optOutput.remove();

   opts.list.name     = "doominati";
   opts.list.nameFull = "Doominati";

   opts.list.version = "v0.0.0";

   opts.list.usage = "[option]...";

   opts.list.descS = "Doominati game engine.";

   try
   {
      if(argc > 1)
         GDCC::Core::ProcessOptions(opts, argc, argv, false);

      std::cout <<
         "Doominati v0.0.0 -- Copyright (C) Team Doominati 2016\n"
         "See COPYING for license information.\n\n";

      return Main();
   }
   catch(std::exception const &e)
   {
      std::cerr << e.what() << std::endl;
      return EXIT_FAILURE;
   }
   catch(int e)
   {
      return e;
   }
}

// EOF

