//--------------------------------------------------------------------------------------------------
/**
 * @file objectFile.h
 *
 * Copyright (C) Sierra Wireless Inc.  Use of this work is subject to license.
 */
//--------------------------------------------------------------------------------------------------

#ifndef LEGATO_MKTOOLS_MODEL_OBJECT_FILE_H_INCLUDE_GUARD
#define LEGATO_MKTOOLS_MODEL_OBJECT_FILE_H_INCLUDE_GUARD

struct ObjectFile_t
{
    std::string path;   ///< Path to the object file relative to the working directory.

    std::string sourceFilePath; ///< Absolute path to the source code file.

    ObjectFile_t(const std::string& p, const std::string& s)
        : path(p), sourceFilePath(s) {}
};


#endif // LEGATO_MKTOOLS_MODEL_OBJECT_FILE_H_INCLUDE_GUARD
