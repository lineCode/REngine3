//
//  TMXSmartPointer.hpp
//  REngine3
//
//  Created by reworks on 30/10/2016.
//  Copyright (c) 2016 reworks. All rights reserved.
//

#ifndef RENGINE3_TMXSMARTPOINTER_HPP_
#define RENGINE3_TMXSMARTPOINTER_HPP_

#include <memory>

#include "re/mapping/tmx/tmx.h"

namespace re
{
	namespace tmx
	{
		/*
		* IMPORTS: map pointer to delete.
		* EXPORTS: none
		* PURPOSE: Custom deleter for tmx_map files.
		*/
		struct TMXDeleter
		{
			void operator()(tmx_map* map) { if (map) tmx_map_free(map); }
		};

		/*
		* IMPORTS: none
		* EXPORTS: none
		* PURPOSE: Creates a smart pointer version of tmx_map to ensure it is deleted properly. IT IS A UNIQUE_PTR!
		*/
		typedef std::unique_ptr<tmx_map, TMXDeleter> Ptr;

		/*
		* IMPORTS: map file. Does not use the VFS.
		* EXPORTS: none
		* PURPOSE: Creates and returns tmx::UPtr with the tmx map parsed.
		*/
		inline tmx::Ptr make(const std::string& map)
		{
			return tmx::Ptr{ tmx_load(map.c_str()) };
		}
	}
}

#endif
