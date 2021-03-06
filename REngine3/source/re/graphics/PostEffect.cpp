//
//  PostEffect.cpp
//  REngine3
//
/*
#####################
SFML Game Development
License
#####################

This file describes the licenses for material accompanying the book "SFML Game Development".
Copyright(c) 2013 Artur Moreira, Henrik Vogelius Hansson, Jan Haller



========== =
Source code
========== =

This software is provided 'as-is', without any express or implied
warranty.In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
excluding commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions :

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. Redistributions may not be sold, nor may they be used in a commercial
product or activity.

4. This notice may not be removed or altered from any source distribution.
*/

// ALTERED SOURCE

#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "PostEffect.hpp"

namespace re
{
	PostEffect::~PostEffect()
	{
	}

	void PostEffect::applyShader(const sf::Shader& shader, sf::RenderTarget& output)
	{
		sf::Vector2f outputSize = static_cast<sf::Vector2f>(output.getSize());

		sf::VertexArray vertices(sf::TrianglesStrip, 4);
		vertices[0] = sf::Vertex(sf::Vector2f(0, 0), sf::Vector2f(0, 1));
		vertices[1] = sf::Vertex(sf::Vector2f(outputSize.x, 0), sf::Vector2f(1, 1));
		vertices[2] = sf::Vertex(sf::Vector2f(0, outputSize.y), sf::Vector2f(0, 0));
		vertices[3] = sf::Vertex(sf::Vector2f(outputSize), sf::Vector2f(1, 0));

		sf::RenderStates states;
		states.shader = &shader;
		states.blendMode = sf::BlendNone;

		output.draw(vertices, states);
	}
}
