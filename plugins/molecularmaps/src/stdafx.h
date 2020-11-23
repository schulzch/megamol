/*
 * stdafx.h
 * Copyright (C) 2006-2015 by MegaMol Team
 * Alle Rechte vorbehalten.
 */

#ifndef MOLECULARMAPS_STDAFX_H_INCLUDED
#define MOLECULARMAPS_STDAFX_H_INCLUDED
#if (defined(_MSC_VER) && (_MSC_VER > 1000))
#pragma once
#endif /* (defined(_MSC_VER) && (_MSC_VER > 1000)) */

#ifdef _WIN32
/* Windows includes */

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#else /* _WIN32 */
/* Linux includes */

#include <memory.h>

#ifndef NULL
#   define NULL 0
#endif

#endif /* _WIN32 */

#include "vislib/types.h"

// VISlib includes
#include "vislib/graphics/gl/ShaderSource.h"
#include "vislib/graphics/gl/GLSLGeometryShader.h"
#include "vislib/graphics/gl/GLSLTesselationShader.h"
#include "vislib/graphics/gl/FramebufferObject.h"
#include "vislib/types.h"
#include "vislib/math/Vector.h"
#include "vislib/math/Matrix.h"
#include "mmcore/utility/log/Log.h"

// C++ includes
#include <array>
#include <vector>
#include <algorithm>
#include <ctime>
#include <mutex>
#include <thread>
#include <cfloat>
#include <set>
#include <iostream>
#include <queue>
#include <valarray>
#include <queue> 
#include <map>
#include <ppl.h>

// MolecularMpas inclues
#include "Types.h"

// MolecularMaps defines
static clock_t begin, end;
#define TIME_START begin = clock();
#define TIME_END(name) end = clock(); \
megamol::core::utility::log::Log::DefaultLog.WriteMsg(megamol::core::utility::log::Log::LEVEL_INFO, "%s took %f seconds.", name, double(end - begin) / CLOCKS_PER_SEC);

#endif /* MOLECULARMAPS_STDAFX_H_INCLUDED */
