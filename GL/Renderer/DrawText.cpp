//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Drawing text.
//
//-----------------------------------------------------------------------------

#include "GL/Renderer/PrivData.hpp"

#include "Code/MemStr.hpp"
#include "Code/Program.hpp"
#include "Code/Native.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Type Definitions                                                           |
//

namespace DGE::GL
{
   //
   // TextLine
   //
   struct TextLine
   {
      TextLine(float width_, char const *beg_, char const *end_) :
         width(width_), beg(beg_), end(end_) {}

      float       width;
      char const *beg, *end;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // Renderer::drawText
   //
   void Renderer::drawText(int x, int y, char const *str, int maxwidth)
   {
      if(!fontCurrent || !str || !*str) return;

      char const *end = str + std::strlen(str);

      std::vector<TextLine> lines;
      float maxlinewidth = 0;

      for(char const *itr = str; itr < end;)
      {
         char const *beg = itr, *lend;
         float width = 0;

         fontCurrent->kernReset();

         for(;;)
         {
            char const *nex;
            char32_t ch; std::tie(ch, nex) = GDCC::Core::Str8To32(itr, end);

            if(ch == '\n') {lend = itr; itr = nex; break;}

            auto &gly = fontCurrent->getChar(ch);
            auto glyphw = gly.ax + fontCurrent->getKernAmount();

            if(maxwidth && width +  glyphw > maxwidth) {lend = itr; break;}
            else           width += glyphw;

            if(nex >= end) {lend = itr = end; break;}
            else                   itr = nex;
         }

         if(width > maxlinewidth) maxlinewidth = width;
         lines.emplace_back(width, beg, lend);
      }

      float height = fontCurrent->getHeight() * lines.size();
      float py;
      float px;
      float origx;

      switch(textAlignV)
      {
      default:
      case AlignVert::Top:    py = y;                   break;
      case AlignVert::Bottom: py = y -  height;         break;
      case AlignVert::Center: py = y - (height / 2.0f); break;
      }

      auto origtex = textureCurrent();

      for(auto &line : lines)
      {
         fontCurrent->kernReset();

         switch(textAlignH)
         {
         default:
         case AlignHorz::Left:      origx = px = x;                                        break;
         case AlignHorz::Right:     origx = px = x -                  line.width;          break;
         case AlignHorz::Center:    origx = px = x -                 (line.width / 2.0f);  break;
         case AlignHorz::RightBox:  origx = px = x + (maxlinewidth -  line.width);         break;
         case AlignHorz::CenterBox: origx = px = x + (maxlinewidth - (line.width / 2.0f)); break;
         }

         for(char const *itr = line.beg; itr < line.end;)
         {
            char32_t ch; std::tie(ch, itr) = GDCC::Core::Str8To32(itr, line.end);

            if(ch == '\r')
               {px = origx; continue;}

            auto &gly = fontCurrent->getChar(ch);

            auto kern = fontCurrent->getKernAmount();
            auto dx = px + gly.ox + kern;
            auto dy = py + gly.oy;

            textureBind(&gly.data);
            drawRectangle(dx, dy, dx + gly.w, dy + gly.h);

            px += gly.ax + kern;
         }

         py += fontCurrent->getHeight();
      }

      textureBind(origtex);
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::GL
{
   //
   // void DGE_DrawText(int x, int y, char const *str)
   //
   DGE_Code_NativeDefn(DGE_DrawText)
   {
      int x = static_cast<int>(argv[0]);
      int y = static_cast<int>(argv[1]);
      Code::MemPtr<Code::Byte const> str = {&task->prog->memory, argv[2]};
      Renderer::Current->drawText(x, y, MemStrDup(str).get());
      return false;
   }

   //
   // void DGE_TextAlignment(int h, int v)
   //
   DGE_Code_NativeDefn(DGE_TextAlignment)
   {
      AlignHorz h = static_cast<AlignHorz>(argv[0]);
      AlignVert v = static_cast<AlignVert>(argv[1]);

      if(h != AlignHorz::Keep) Renderer::Current->textAlignH = h;
      if(v != AlignVert::Keep) Renderer::Current->textAlignV = v;

      return false;
   }
}

// EOF

