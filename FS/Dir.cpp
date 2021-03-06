//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Filesystem directory abstraction.
//
//-----------------------------------------------------------------------------

#include "FS/Dir.hpp"

#include <GDCC/Core/Dir.hpp>
#include <GDCC/Core/File.hpp>
#include <GDCC/Core/Option.hpp>
#include <GDCC/Core/String.hpp>

#include <vector>
#include <tuple>
#include <cctype>


//----------------------------------------------------------------------------|
// Options                                                                    |
//

namespace DGE::FS
{
   //
   // --fs-extra
   //
   GDCC::Option::Bool Dir::Extra
   {
      &GDCC::Core::GetOptionList(), GDCC::Option::Base::Info()
         .setName("fs-extra")
         .setGroup("filesystem")
         .setDescS("Enables extra filesystem features."),
      true
   };

   //
   // --fs-varchive
   //
   GDCC::Option::CStr Dir::VarArchive
   {
      &GDCC::Core::GetOptionList(), GDCC::Option::Base::Info()
         .setName("fs-varchive")
         .setGroup("filesystem")
         .setDescS("Sets archive to store writeable filesystem.")
   };
}


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::FS
{
   std::unique_ptr<Dir> Dir::Root;
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::FS
{
   //
   // Dir::createDir
   //
   bool Dir::createDir(Core::HashedStr)
   {
      return false;
   }

   //
   // Dir::createDirPath
   //
   bool Dir::createDirPath(Core::HashedStr path)
   {
      if(auto sep = static_cast<char const *>(std::memchr(path.str, '/', path.len)))
      {
         if(auto dir = findDir({path.str, sep}))
            return dir->createDirPath({sep+1, path.end()});

         return false;
      }
      else
         return createDir(path);
   }

   //
   // Dir::createFile
   //
   Dir::FilePtr Dir::createFile(Core::HashedStr)
   {
      return nullptr;
   }

   //
   // Dir::createFilePath
   //
   Dir::FilePtr Dir::createFilePath(Core::HashedStr path)
   {
      if(auto sep = static_cast<char const *>(std::memchr(path.str, '/', path.len)))
      {
         if(auto dir = findDir({path.str, sep}))
            return dir->createFilePath({sep+1, path.end()});

         return nullptr;
      }
      else
         return createFile(path);
   }

   //
   // Dir::findDir
   //
   Dir::DirPtr Dir::findDir(Core::HashedStr dirname)
   {
      FilePtr file = findFile(dirname);
      return {file ? file->findDir() : nullptr, false};
   }

   //
   // Dir::findDirPath
   //
   Dir::DirPtr Dir::findDirPath(Core::HashedStr path)
   {
      if(auto sep = static_cast<char const *>(std::memchr(path.str, '/', path.len)))
      {
         if(auto dir = findDir({path.str, sep}))
            return dir->findDirPath({sep+1, path.end()});

         return {nullptr, false};
      }
      else
         return findDir(path);
   }

   //
   // Dir::findFilePath
   //
   Dir::FilePtr Dir::findFilePath(Core::HashedStr path)
   {
      if(auto sep = static_cast<char const *>(std::memchr(path.str, '/', path.len)))
      {
         if(auto dir = findDir({path.str, sep}))
            return dir->findFilePath({sep+1, path.end()});

         return nullptr;
      }
      else
         return findFile(path);
   }

   //
   // Dir::flush
   //
   void Dir::flush()
   {
   }

   //
   // Dir::forFile
   //
   void Dir::forFile(FileFunc const &fn)
   {
      for(auto iter : *this)
      {
         if(iter.dir) iter.dir->forFile(fn);
         else if(iter.file) fn(iter.file);
      }
   }

   //
   // Dir::forFilePath
   //
   void Dir::forFilePath(Core::HashedStr path, FileFunc const &fn)
   {
      if(auto sep = static_cast<char const *>(std::memchr(path.str, '/', path.len)))
      {
         if(auto dir = findDir({path.str, sep}))
            dir->forFilePath({sep+1, path.end()}, fn);
      }
      else if(auto dir = findDir(path))
         dir->forFile(fn);
      else if(auto file = findFile(path))
         fn(file);
   }

   //
   // Dir::hasName
   //
   bool Dir::hasName(Core::HashedStr filename)
   {
      return findFile(filename) || findDir(filename);
   }

   //
   // Dir::iterGetDir
   //
   Dir::DirPtr Dir::iterGetDir(IterData iter)
   {
      return iterGet(iter).dir;
   }

   //
   // Dir::iterGetFile
   //
   Dir::FilePtr Dir::iterGetFile(IterData iter)
   {
      return iterGet(iter).file;
   }

   //
   // Dir::iterGetName
   //
   Core::HashedStr Dir::iterGetName(IterData iter)
   {
      return iterGet(iter).name();
   }

   //
   // Dir::iterNext
   //
   Dir::IterData Dir::iterNext(IterData iter)
   {
      return iter + 1;
   }

   //
   // Dir::prune
   //
   void Dir::prune()
   {
   }

   //
   // Dir::removeDir
   //
   bool Dir::removeDir(Core::HashedStr)
   {
      return false;
   }

   //
   // Dir::removeDirPath
   //
   bool Dir::removeDirPath(Core::HashedStr path)
   {
      if(auto sep = static_cast<char const *>(std::memchr(path.str, '/', path.len)))
      {
         if(auto dir = findDir({path.str, sep}))
            return dir->removeDirPath({sep+1, path.end()});

         return false;
      }
      else
         return removeDir(path);
   }

   //
   // Dir::removeFile
   //
   bool Dir::removeFile(Core::HashedStr)
   {
      return false;
   }

   //
   // Dir::removeFilePath
   //
   bool Dir::removeFilePath(Core::HashedStr path)
   {
      if(auto sep = static_cast<char const *>(std::memchr(path.str, '/', path.len)))
      {
         if(auto dir = findDir({path.str, sep}))
            return dir->removeFilePath({sep+1, path.end()});

         return false;
      }
      else
         return removeFile(path);
   }

   //
   // DirData default constructor
   //
   DirData::DirData() : fileFS{nullptr}, fileBlock{nullptr} {}

   //
   // DirData move constructor
   //
   DirData::DirData(DirData &&dd) :
      fileFS{dd.fileFS}, fileBlock{std::move(dd.fileBlock)}
   {
      dd.fileFS = nullptr;
   }

   //
   // DirData constructor
   //
   DirData::DirData(File *file) : fileFS{file}
   {
      ++fileFS->refs;
   }

   //
   // DirData constructor
   //
   DirData::DirData(std::unique_ptr<GDCC::Core::FileBlock> &&file) :
      fileFS{nullptr}, fileBlock{std::move(file)}
   {
   }

   //
   // DirData destructor
   //
   DirData::~DirData()
   {
      if(fileFS) --fileFS->refs;
   }

   //
   // DirData::operator = DirData
   //
   DirData &DirData::operator = (DirData &&dd)
   {
      std::swap(fileFS, dd.fileFS);
      fileBlock = std::move(dd.fileBlock);

      return *this;
   }

   //
   // DirData::data
   //
   char const *DirData::data() const
   {
      if(fileFS)    return fileFS->data;
      if(fileBlock) return fileBlock->data();

      return nullptr;
   }

   //
   // DirData::size
   //
   std::size_t DirData::size() const
   {
      if(fileFS)    return fileFS->size;
      if(fileBlock) return fileBlock->size();

      return 0;
   }

   //
   // CreateDir
   //
   std::unique_ptr<Dir> CreateDir(char const *name)
   {
      if(!GDCC::Core::IsDir(name))
      {
         auto file = GDCC::Core::FileOpenBlock(name);

         if(file) switch(DetectFormat(file->data(), file->size()))
         {
         case Format::Pak: return CreateDir_Pak(std::move(file)); break;
       //case Format::Wad: return CreateDir_Wad(std::move(file)); break;
         case Format::Zip: return CreateDir_Zip(std::move(file)); break;

         default:
            return nullptr;
         }
         else
            return nullptr;
      }
      else
         return CreateDir_Directory(name);
   }
}

// EOF

