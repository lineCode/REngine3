//
//  ServiceLocator.hpp
//  REngine3
//
//  Created by reworks on 17/07/2016.
//  Copyright � 2016 reworks. All rights reserved.
//

#ifndef RENGINE3_SERVICELOCATOR_HPP_
#define RENGINE3_SERVICELOCATOR_HPP_

#include <typeindex>
#include <unordered_map>

#include "re/utility/Log.hpp"
#include "re/types/Service.hpp"

namespace re
{
	class Locator
	{
	public:
		/*
		* IMPORTS: none
		* EXPORTS: none
		* PURPOSE: Clean up the locator.
		*/
		~Locator();

		/*
		* IMPORTS: <T> - The type of object, s - The service object reference.
		* EXPORTS: none
		* PURPOSE: To add an engine service to the engines locator.
		*/
		template<typename T>
		static void provide(Service* s);

		/*
		* IMPORTS: <T> - The type of object
		* EXPORTS: none
		* PURPOSE: To retrieve an engine service.
		*/
		template<typename T>
		static T* get();

		/*
		* IMPORTS: <T> - The type of object
		* EXPORTS: none
		* PURPOSE: To remove a service from the locator (does not delete the original object, that is handled by Application class).
		*/
		template<typename T>
		static void remove();

	private:
		static std::unordered_map<std::type_index, Service*> m_services;

	};

	template<typename T>
	void Locator::provide(Service* s)
	{
		// Find type in the map.
		auto it = m_services.find(std::type_index(typeid(T)));

		if (it != m_services.end()) 
		{
			RE_LOG(LogLevel::WARNING, "Tried to provide an already existing service", "Locator::provide", "ServiceLocator.hpp", 67);
		}
		else 
		{
			m_services[typeid(T)] = s;
		}
	}

	template<typename T>
	T* Locator::get()
	{
		// Find type in the map.
		auto it = m_services.find(std::type_index(typeid(T)));

		if (it != m_services.end()) 
		{
			// Cast to required type and return.
			return dynamic_cast<T*>(it->second);
		}
		else 
		{
			return nullptr;
		}
	}

	template<typename T>
	void Locator::remove()
	{
		// Find type in the map.
		auto it = m_services.find(std::type_index(typeid(T)));

		if (it != m_services.end())
		{
			// Erase from map.
			m_services.erase(typeid(T));
		}
		else
		{
			RE_LOG(LogLevel::WARNING, "Attempted to remove a service that doesnt exist", "Locator::remove", "ServiceLocator.hpp", 105);
		}
	}
}

#endif
