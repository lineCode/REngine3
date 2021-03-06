//
//  AudioSystem.cpp
//  REngine3
//
//  Created by reworks on 10/11/2016.
//  Copyright (c) 2016 reworks. All rights reserved.
//

#include "re/app/World.hpp"
#include "re/entity/Entity.hpp"
#include "re/component/SoundComponent.hpp"
#include "re/component/MusicComponent.hpp"

#include "AudioSystem.hpp"

namespace re
{
	AudioSystem::AudioSystem(int defaultVolume)
	{
		m_defaultVolume = defaultVolume;
	}

	AudioSystem::~AudioSystem()
	{
		m_entitys.clear();
	}

	void AudioSystem::submit(World* world)
	{
		for (auto& it : world->getAliveEntitys())
		{
			if (it.second.has<MusicComponent>() || it.second.has<SoundComponent>())
			{
				addEntity(&it.second);
			}
		}

		setGlobalMusicVolume(m_defaultVolume);
		setGlobalSoundVolume(m_defaultVolume);
	}

	void AudioSystem::addEntity(Entity* e)
	{
		if (e->m_systemIds.find("AudioSystem") == e->m_systemIds.end())
		{
			e->m_systemIds.emplace("AudioSystem", std::type_index(typeid(AudioSystem)));
		}
		
		m_entitys.emplace(e->m_name, e);
	}

	void AudioSystem::removeEntity(const std::string& name)
	{
		auto found = m_entitys.find(name);

		if (found != m_entitys.end())
		{
			m_entitys.erase(name);
		}
	}

	sf::Sound& AudioSystem::getSound(const std::string& accessor)
	{
		auto lastdot = accessor.find_last_of(".");
		std::string name = accessor.substr(0, lastdot);
		std::string sound = accessor.substr(lastdot + 1);

		return *(m_entitys[name]->get<SoundComponent>()->m_sounds[sound].second);
	}

	sf::Music& AudioSystem::getMusic(const std::string& accessor)
	{
		auto lastdot = accessor.find_last_of(".");
		std::string name = accessor.substr(0, lastdot);
		std::string music = accessor.substr(lastdot + 1);

		return *(m_entitys[name]->get<MusicComponent>()->m_music[music]);
	}

	void AudioSystem::setGlobalMusicVolume(float volume)
	{
		for (auto& e : m_entitys)
		{
			if (e.second->has<MusicComponent>())
			{
				auto* map = &(e.second->get<MusicComponent>()->m_music);
				for (auto it = map->begin(); it != map->end(); it++)
				{
					float ov = it->second->getVolume();
					float nv = (volume + ov) / 2.0f;
					it->second->setVolume(nv);
				}
			}
		}
	}

	void AudioSystem::setGlobalSoundVolume(float volume)
	{
		for (auto& e : m_entitys)
		{
			if (e.second->has<SoundComponent>())
			{
				auto* map = &(e.second->get<SoundComponent>()->m_sounds);
				for (auto it = map->begin(); it != map->end(); it++)
				{
					float ov = it->second.second->getVolume();
					float nv = (volume + ov) / 2.0f;
					it->second.second->setVolume(nv);
				}
			}
		}
	}

	void AudioSystem::stop()
	{
		for (auto& e : m_entitys)
		{
			if (e.second->has<MusicComponent>())
			{
				auto* map = &(e.second->get<MusicComponent>()->m_music);
				for (auto it = map->begin(); it != map->end(); it++)
				{
					it->second->stop();
				}
			}

			if (e.second->has<SoundComponent>())
			{
				auto* map = &(e.second->get<SoundComponent>()->m_sounds);
				for (auto it = map->begin(); it != map->end(); it++)
				{
					it->second.second->stop();
				}
			}
		}
	}

	void AudioSystem::clean()
	{
		stop();

		m_entitys.clear();
	}
}
