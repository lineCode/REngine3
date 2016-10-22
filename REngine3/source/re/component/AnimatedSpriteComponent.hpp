//
//  AnimatedSpriteComponent.hpp
//  REngine3
//
//  Created by reworks on 16/08/2016.
//  Copyright (c) 2016 reworks. All rights reserved.
//

#ifndef RENGINE3_ANIMATEDSPRITECOMPONENT_HPP_
#define RENGINE3_ANIMATEDSPRITECOMPONENT_HPP_

#include "re/types/Component.hpp"
#include "re/graphics/Animation.hpp"
#include "re/graphics/AnimatedSprite.hpp"

#include "re/services/vfs/sfmlphysfs.hpp"

namespace re
{
	class AnimatedSpriteComponent : public Component, public AnimatedSprite
	{
	public:
		/*
		* IMPORTS: none
		* EXPORTS: none
		* PURPOSE: Default Constructor.
		*/
		AnimatedSpriteComponent();

		/*
		* IMPORTS: none
		* EXPORTS: none
		* PURPOSE: Destructor. Cleans up component.
		*/
		~AnimatedSpriteComponent() override;

		/*
		* IMPORTS: sol::table from lua script containing component data.
		* EXPORTS: none
		* PURPOSE: Set up the component.
		*/
		void Init(sol::table& table) override;
		
		/*
		* IMPORTS: animantion name in string
		* EXPORTS: none
		* PURPOSE: Set the components current active animation.
		*/
		void SetActiveAnimation(const std::string& animation);

		/*
		* IMPORTS: A change in position if nessessary. Delta time aswell.
		* EXPORTS: none
		* PURPOSE: Update the component.
		*/
		void Update(sf::Time dt, float x, float y) override;
		
		/*
		* IMPORTS: none
		* EXPORTS: none
		* PURPOSE: Play the current animation.
		*/
		void Play();

	private:
		/*
		* IMPORTS: RenderTarget & RenderStates
		* EXPORTS: none
		* PURPOSE: draw to target using states. See SFML documentation.
		*/
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	private:
		float m_speed;

		sf::physfs m_animationSheetStream;
		sf::Texture m_animationSheet;

		std::unordered_map<std::string, Animation> m_animations;
		std::string m_activeAnimation;

	public:
		unsigned long m_group;
	};
}

#endif