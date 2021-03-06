//
//  StateManager.cpp
//  REngine3
//
//  Created by reworks on 5/10/2016.
//  Copyright (c) 2016 reworks. All rights reserved.
//

#include "re/app/World.hpp"
#include "re/services/ServiceLocator.hpp"

#include "StateManager.hpp"

namespace re
{
	State::~State()
	{
	}

	void StateManager::changeState(std::shared_ptr<State> s)
	{
	    m_currentState->unloadResources();
		m_currentState = s;
        Locator::get<World>()->entitysHaveChanged();
		m_currentState->loadResources();
	}

	void StateManager::reloadState()
	{
		m_currentState->unloadResources();
		Locator::get<World>()->entitysHaveChanged();
		m_currentState->loadResources();
	}

	void StateManager::setState(std::shared_ptr<State> s)
	{
		m_currentState = s;
	}

	void StateManager::handlePollEvents(sf::Event& event)
	{
		m_currentState->handlePollEvents(event);
	}

    void StateManager::handleEvents(sf::Event &event)
    {
        m_currentState->handleEvents(event);
    }
    
	void StateManager::update(sf::Time dt)
	{
		m_currentState->update(dt);
	}

	void StateManager::render()
	{
		m_currentState->render();
	}

	void StateManager::loadResources()
	{
		m_currentState->loadResources();
	}

	void StateManager::unloadResources()
	{
		m_currentState->unloadResources();
	}
}
