//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// HashMapFixed class.
//
//-----------------------------------------------------------------------------

#ifndef Doom__Core__HashMapFixed_H__
#define Doom__Core__HashMapFixed_H__

#include <functional>
#include <new>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Core
   {
      //
      // HashMapFixed
      //
      // Non-resizable hash map.
      // Also acts as an array for element access by index.
      //
      template<typename Key, typename T, typename Hash = std::hash<Key>>
      class HashMapFixed
      {
      public:
         struct Elem
         {
            Key   key;
            T     val;
            Elem *next;
         };

         using const_iterator = Elem const *;
         using iterator       = Elem *;
         using size_type      = std::size_t;
         using value_type     = Elem;


         HashMapFixed() : hasher{}, table{nullptr}, elemV{nullptr}, elemC{0} {}
         ~HashMapFixed() {free();}

         // operator [ std::size_t ]
         T       &operator [] (std::size_t i)       {return elemV[i].val;}
         T const &operator [] (std::size_t i) const {return elemV[i].val;}

         //
         // alloc
         //
         void alloc(size_type count)
         {
            if(elemV) free();

            if(!count) return;

            size_type sizeRaw = sizeof(Elem) * count + sizeof(Elem *) * count;

            elemC = count;
            elemV = static_cast<Elem *>(::operator new(sizeRaw));
         }

         // begin
         iterator       begin()       {return elemV;}
         const_iterator begin() const {return elemV;}

         //
         // build
         //
         void build()
         {
            // Initialize table.
            table = reinterpret_cast<Elem **>(elemV + elemC);
            for(Elem **elem = table + elemC; elem != table;)
               *--elem = nullptr;

            // Insert elements.
            for(Elem &elem : *this)
            {
               size_type hash = hasher(elem.key) % elemC;

               elem.next = table[hash];
               table[hash] = &elem;
            }
         }

         // empty
         bool empty() const {return !elemC;}

         // end
         iterator       end()       {return elemV + elemC;}
         const_iterator end() const {return elemV + elemC;}

         //
         // find
         //
         T *find(Key const &key)
         {
            if(!table) return nullptr;

            for(Elem *elem = table[hasher(key) % elemC]; elem; elem = elem->next)
            {
               if(elem->key == key)
                  return &elem->val;
            }

            return nullptr;
         }

         //
         // free
         //
         void free()
         {
            if(table)
            {
               for(Elem *elem = elemV + elemC; elem != elemV;)
                  (--elem)->~Elem();

               table = nullptr;
            }

            ::operator delete(elemV);

            elemV = nullptr;
            elemC = 0;
         }

         // size
         size_type size() const {return elemC;}

      private:
         Hash hasher;

         Elem    **table;
         Elem     *elemV;
         size_type elemC;
      };
   }
}

#endif//Doom__Core__HashMapFixed_H__

