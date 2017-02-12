//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Floating-point 2-dimensional vector.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__Vector2_H__
#define DGE__Core__Vector2_H__

#include <cmath>
#include "Core/Math.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Core
{
   //
   // Vector2
   //
   class Vector2
   {
   public:
      float x, y;

      Vector2() = default;
      constexpr Vector2(float x_, float y_) : x{x_}, y{y_} {}

      // in-place operations
      Vector2 &add(Vector2 const &other) {x += other.x; y += other.y; return *this;}
      Vector2 &sub(Vector2 const &other) {x -= other.x; y -= other.y; return *this;}

      Vector2 &interpolate(Vector2 const &other, float amt)
      {
         x = Core::Lerp(x, other.x, amt);
         y = Core::Lerp(y, other.y, amt);
         return *this;
      }

      // functional operations
      float length()                  const {return std::sqrt((x * x) + (y * y));}
      float dot(Vector2 const &other) const {return (x * other.x) + (y * other.y);}

      // operator overloads
      explicit operator bool() const {return x && y;}

      Vector2  operator-() const {return {-x, -y};}
      bool     operator!() const {return !(x && y);}

      Vector2  operator +(Vector2 const &other) const {return {x + other.x, y + other.y};}
      Vector2  operator -(Vector2 const &other) const {return {x - other.x, y - other.y};}
      float    operator *(Vector2 const &other) const {return dot(other);}
      Vector2 &operator+=(Vector2 const &other)       {return add(other);}
      Vector2 &operator-=(Vector2 const &other)       {return sub(other);}
   };
}

#endif//DGE__Core__Vector2_H__

