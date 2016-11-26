//
//  AnimationComponent.cpp
//  REngine3
//
//  Created by reworks on 16/08/2016.
//  Copyright (c) 2016 reworks. All rights reserved.
// 
// Code adapted from here: https://github.com/SFML/SFML/wiki/Source:-AnimatedSprite

#include <map>

#include "re/scripting/sol2/sol.hpp"

#include "AnimationComponent.hpp"

namespace re
{
	AnimationComponent::AnimationComponent()
	{
	}

	AnimationComponent::~AnimationComponent()
	{
		m_animations.clear();
	}

	void AnimationComponent::Init(sol::table& table)
	{
		m_animation = NULL;
		m_frameTime = sf::seconds(table.get<float>("frameTime"));
		m_speed = table.get<float>("speed");
		m_currentFrame = 0;
		m_isPaused = table.get<bool>("isPaused");
		m_isLooped = table.get<bool>("isLooped");
		m_texture = NULL;

		m_animationSheetStream.open(table.get<std::string>("texture"));
		m_animationSheet.loadFromStream(m_animationSheetStream);

		SetActiveAnimation(table.get<std::string>("defaultAnim"));

		m_animations.clear();
		sol::table animTable = table.get<sol::table>("Animations");

		std::map<std::string, sol::table> m_keyValuePairAnimations;
		animTable.for_each([&](std::pair<sol::object, sol::object> pair) {
			m_keyValuePairAnimations.insert({ pair.first.as<std::string>(), pair.second.as<sol::table>() });
		});

		for (auto& it : m_keyValuePairAnimations)
		{
			std::map<std::string, sol::table> m_keyValuePairFrames;
			it.second.for_each([&](std::pair<sol::object, sol::object> pair) {
				m_keyValuePairFrames.insert({ pair.first.as<std::string>(), pair.second.as<sol::table>() });
			});

			Animation temp;
			temp.setSpriteSheet(m_animationSheet);

			for (auto& frames : m_keyValuePairFrames)
			{
				temp.addFrame(sf::IntRect(frames.second.get<int>("x"), frames.second.get<int>("y"), frames.second.get<int>("w"), frames.second.get<int>("h")));
			}

			m_animations.emplace(it.first, temp);
		}
	}

	void AnimationComponent::ChangeAnimation(const std::string& animation)
	{
		m_activeAnimation = animation;
	}

	void AnimationComponent::Update(sf::Time dt)
	{
		// from AnimatedSprite.hpp

		// if not paused and we have a valid animation
		if (!m_isPaused && m_animation)
		{
			// add delta time
			m_currentTime += dt;

			// if current time is bigger then the frame time advance one frame
			if (m_currentTime >= m_frameTime)
			{
				// reset time, but keep the remainder
				m_currentTime = sf::microseconds(m_currentTime.asMicroseconds() % m_frameTime.asMicroseconds());

				// get next Frame index
				if (m_currentFrame + 1 < m_animation->getSize())
					m_currentFrame++;
				else
				{
					// animation has ended
					m_currentFrame = 0; // reset to start

					if (!m_isLooped)
					{
						m_isPaused = true;
					}

				}

				// set the current frame, not reseting the time
				setFrame(m_currentFrame, false);
			}
		}
	}

	void AnimationComponent::Play()
	{
		play(m_animations[m_activeAnimation]);
	}
}