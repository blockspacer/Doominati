//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Texture management.
//
//-----------------------------------------------------------------------------

#ifndef DGE__GL__Texture_H__
#define DGE__GL__Texture_H__

#include "../GL/Types.hpp"

#include "../Core/Types.hpp"

#include <exception>
#include <stdexcept>
#include <memory>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::GL
{
   using Texture = Core::Resource<TextureData>;

   //
   // TextureData
   //
   class TextureData
   {
   public:
      TextureData() : w{0}, h{0}, tex{0} {}
      TextureData(TextureData const &) = delete;
      TextureData(TextureData &&t) : w{t.w}, h{t.h}, tex{t.tex} {t.tex = 0;}
      TextureData(TextureDim w, TextureDim h, TexturePixel const *texdata);
      ~TextureData();

      TextureData &operator = (TextureData const &) = delete;
      TextureData &operator = (TextureData &&t)
         {std::swap(tex, t.tex); return *this;}

      TextureDim w, h;

      GLuint tex;
   };

   //
   // TextureLoader
   //
   class TextureLoader
   {
   public:
      virtual ~TextureLoader() {}

      virtual void data(TexturePixel *buf) = 0;

      virtual std::pair<TextureDim, TextureDim> size() = 0;
   };

   //
   // TextureLoaderError
   //
   class TextureLoaderError : public std::runtime_error
   {
   public:
      using std::runtime_error::runtime_error;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   std::unique_ptr<TextureLoader> CreateTextureLoader(FS::File *file);
}

#endif//DGE__GL__Texture_H__

