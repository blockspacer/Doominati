//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Program entry point.
//
//-----------------------------------------------------------------------------

#if DGE_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#define DGE_doominati_InitMain() SDL_SetMainReady()
#else
#define DGE_doominati_InitMain()
#endif

#include "AL/AudioRenderer.hpp"

#include "Code/Codedefs.hpp"
#include "Code/Native.hpp"
#include "Code/Process.hpp"
#include "Code/Program.hpp"
#include "Code/Thread.hpp"

#include "Core/Time.hpp"

#include "FS/Dir.hpp"

#include "GL/Window.hpp"
#include "GL/Renderer.hpp"

#include "Game/Input.hpp"
#include "Game/Thinker.hpp"

#include <GDCC/Core/Option.hpp>

#include <GDCC/Option/Bool.hpp>

#include "SDL.h"

#include <iostream>
#include <cstdlib>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game {void NativeAdd();}
namespace DGE::GL   {void NativeAdd();}


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// LoadCodedefs
//
static void LoadCodedefs(DGE::Code::Program *prog)
{
   // Finish internal loader data.
   DGE::Game::NativeAdd();
   DGE::GL::NativeAdd();

   DGE::Code::NativeAdder::Finish();
   DGE::Game::Object::ObjectTypeAdder::Finish();

   // Load CODEDEFS files.
   DGE::Code::Loader loader;
   DGE::FS::Dir::ForFile("codedefs",
      std::bind(&DGE::Code::Loader::loadCodedefs, &loader, std::placeholders::_1));

   // Print status.
   if(loader.loadPASS)
   {
      char const *s = (loader.loadPASS != 1) ? "s" : "";
      std::cerr << "Loaded " << loader.loadPASS << " codedef" << s << ".\n";
   }

   if(loader.loadFAIL)
   {
      char const *s = (loader.loadFAIL != 1) ? "s" : "";
      std::cerr << "Encountered " << loader.loadFAIL <<
         " codedefs error" << s << ".\n";
      throw EXIT_FAILURE;
   }

   // Finish external loader data.
   for(auto &em : DGE::Code::ExtMems) em.val->finish();

   // Process loaded CODEDEFS.
   loader.gen(prog);
}

//
// Main
//
static int Main()
{
   // Initialize SDL.
   class SDLHandler
   {
   public:
      SDLHandler()
      {
         if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
         {
            std::cerr << "SDL_Init: " << SDL_GetError() << std::endl;
            throw EXIT_FAILURE;
         }
      }

      ~SDLHandler()
      {
         SDL_Quit();
      }
   };

   SDLHandler sdl;

   // Initialize filesystem.
   if(GDCC::Core::GetOptionArgs().size())
   {
      for(auto const &arg : GDCC::Core::GetOptionArgs())
         DGE::FS::Dir::AddRoot(arg);
   }
   else
      DGE::FS::Dir::AddRoot(".");

   // Initialize rendering.
   DGE::GL::Window window{640, 480};
   DGE::GL::Renderer renderer{window};

   DGE::GL::Window::SetCurrent(&window);
   DGE::GL::Renderer::SetCurrent(&renderer);

   // Initialize audio.
   DGE::AL::AudioRenderer audio;

   DGE::AL::AudioRenderer::SetCurrent(&audio);

   // Initialize input.
   DGE::Game::InputSource_Local input;

   DGE::Game::InputSource::SetCurrent(&input);

   // Initialize scripting and call main.
   DGE::Code::Program prog;
   LoadCodedefs(&prog);
   DGE::Code::Process proc{&prog};

   if(auto func = prog.funcs.find("main"))
      proc.mainThread()->startTask(func, nullptr, 0);

   // Run main loop.
   std::size_t timeLast = DGE::Core::GetTicks<DGE::Core::PlayTick<>>();
   std::size_t timeNext;

   for(;;)
   {
      std::size_t timeDelta;

      timeNext  = DGE::Core::GetTicks<DGE::Core::PlayTick<>>();
      timeDelta = timeNext - timeLast;
      timeLast  = timeNext;

      DGE::Game::PumpEvents();

      if(timeDelta)
      {
         while(timeDelta--)
         {
            input.poll();
            DGE::Game::Thinker::ThinkAll();
            proc.exec();
            DGE::Game::EventFrame();
         }
      }
      else
         SDL_Delay(1);

      // Rendering actions.
      renderer.renderBegin();
      renderer.render();
      renderer.renderEnd();
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
   DGE_doominati_InitMain();

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
         " Doominati " << opts.list.version <<
         " Copyright (C) 2017 Team Doominati\n"
         "      See COPYING for license information.\n\n";

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

