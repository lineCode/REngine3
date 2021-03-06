//
//  TMXMap.cpp
//  REngine3
//
//  Created by reworks on 30/10/2016.
//  Copyright (c) 2016 reworks. All rights reserved.
//

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "re/utility/Log.hpp"
#include "re/graphics/sw/Line.hpp"
#include "re/graphics/EllipseShape.hpp"
#include "re/component/TransformComponent.hpp"

#include "TMXMap.hpp"

#define SF_THICKNESS 2.5
#define SF_ANIMATION_DELAY 11.1

namespace re
{

	void* sf_tex_loader(const char *path)
	{
		// We allocate on the heap because we need to retain this as long as the map is valid.
		sf::Texture* tex = new sf::Texture;
		tex->loadFromFile(path);
		return (void*)tex;
	}

	void sf_tex_deleter(void* data)
	{
		delete (sf::Texture*)data;
	}

	sf::Color TMXMap::int_to_sf_colour(int colour)
	{
		unsigned char r, g, b;

		// Use bit operations to calculate rgb values.
		r = (colour >> 16) & 0xFF;
		g = (colour >> 8) & 0xFF;
		b = (colour) & 0xFF;

		return sf::Color(r, g, b, 255);
	}

	void TMXMap::draw_polyline(double **points, double x, double y, int pointsc, sf::Color col)
	{
		int i;
		for (i = 1; i<pointsc; i++)
		{
			sw::Line line(sf::Vector2f((float)(x + points[i - 1][0]), (float)(y + points[i - 1][1])), sf::Vector2f((float)(x + points[i][0]), (float)(y + points[i][1])), SF_THICKNESS, col);
			
			m_batchTexture.draw(line);
		}
	}

	void TMXMap::draw_polygon(double **points, double x, double y, int pointsc, sf::Color col)
	{
		draw_polyline(points, x, y, pointsc, col);
		if (pointsc > 2)
		{
			sw::Line line(sf::Vector2f((float)(x + points[0][0]), (float)(y + points[0][1])), sf::Vector2f((float)(x + points[pointsc - 1][0]), (float)(y + points[pointsc - 1][1])), SF_THICKNESS, col);

			m_batchTexture.draw(line);
		}
	}

	void TMXMap::draw_objects(tmx_object_group *objgr)
	{
		sf::Color col = int_to_sf_colour(objgr->color);
		sf::RectangleShape rect;
		re::EllipseShape elip;

		tmx_object *head = objgr->head;
		while (head)
		{
			if (head->visible)
			{
				if (head->shape == S_SQUARE)
				{
					rect.setPosition((float)head->x, (float)head->y);
					rect.setSize(sf::Vector2f((float)head->width, (float)head->height));
					rect.setFillColor(sf::Color::Transparent);
					rect.setOutlineColor(col);
					rect.setOutlineThickness(SF_THICKNESS);
					
					m_batchTexture.draw(rect);
				}
				else if (head->shape == S_POLYGON)
				{
					draw_polygon(head->points, head->x, head->y, head->points_len, col);
				}
				else if (head->shape == S_POLYLINE)
				{
					draw_polyline(head->points, head->x, head->y, head->points_len, col);
				}
				else if (head->shape == S_ELLIPSE)
				{
					// may need to change
					elip.setPosition(sf::Vector2f((float)head->x, (float)head->y));
					elip.setRadius(sf::Vector2f((float)(head->width / 2.0), (float)(head->height / 2.0)));
					elip.setFillColor(sf::Color::Transparent);
					elip.setOutlineColor(col);
					elip.setOutlineThickness(SF_THICKNESS);

					m_batchTexture.draw(elip);
				}
			}

			head = head->next;
		}
	}

	unsigned int TMXMap::gid_clear_flags(unsigned int gid)
	{
		return gid & TMX_FLIP_BITS_REMOVAL;
	}

	void TMXMap::draw_layer(tmx_map *map, tmx_layer *layer)
	{
		unsigned long i, j;
		unsigned int gid;
		double op;

		sf::Texture* tilesetImage = nullptr;
		tmx_tileset *ts;
		tmx_image *im;

		op = layer->opacity;
		for (i = 0; i < map->height; i++)
		{
			for (j = 0; j < map->width; j++)
			{
				gid = gid_clear_flags(layer->content.gids[(i*map->width) + j]);
				if (map->tiles[gid] != NULL)
				{										
					if (!map->tiles[gid]->animation)
					{
						ts = map->tiles[gid]->tileset;
						im = map->tiles[gid]->image;

						if (im)
						{
							tilesetImage = (sf::Texture*)im->resource_image;
						}
						else
						{
							tilesetImage = (sf::Texture*)ts->image->resource_image;
						}
						
						sf::Sprite tileSprite;
						tileSprite.setTexture(*(tilesetImage));
						tileSprite.setPosition((float)(j*ts->tile_width), (float)(i*ts->tile_height));
						tileSprite.setTextureRect(sf::IntRect(map->tiles[gid]->ul_x, map->tiles[gid]->ul_y, ts->tile_width, ts->tile_height));
						m_batchTexture.draw(tileSprite);
					}
				}
			}
		}
	}

	void TMXMap::drawAnimationLayer(tmx_map *map, tmx_layer *layer, sf::Time dt)
	{
		unsigned long i, j;
		unsigned int gid;
		double op;

		sf::Texture* tilesetImage = nullptr;
		tmx_tileset *ts;
		tmx_image *im;
		tmx_tile* focusTile;

		op = layer->opacity;
		for (i = 0; i < map->height; i++)
		{
			for (j = 0; j < map->width; j++)
			{
				gid = gid_clear_flags(layer->content.gids[(i*map->width) + j]);
				focusTile = map->tiles[gid];
				
				if (focusTile != NULL)
				{
					if (focusTile->animation)
					{
						focusTile->animation_timer += dt.asMilliseconds();

						if (focusTile->animation_timer >= (focusTile->animation[focusTile->current_animation].duration*SF_ANIMATION_DELAY))
						{
							focusTile->animation_timer = 0;
							focusTile->current_animation++;
						}

						if (focusTile->current_animation >= focusTile->animation_len)
						{
							focusTile->current_animation = 0;
						}

						int tx = m_map->tiles[focusTile->animation[focusTile->current_animation].tile_id + 1]->ul_x;
						int ty = m_map->tiles[focusTile->animation[focusTile->current_animation].tile_id + 1]->ul_y;

						im = focusTile->image;
						ts = focusTile->tileset;

						if (im)
						{
							tilesetImage = (sf::Texture*)im->resource_image;
						}
						else
						{
							tilesetImage = (sf::Texture*)ts->image->resource_image;
						}

						sf::Sprite tileSprite;
						tileSprite.setTexture(*(tilesetImage));
						tileSprite.setPosition((float)(j*ts->tile_width), (float)(i*ts->tile_height));
						tileSprite.setTextureRect(sf::IntRect(tx, ty, ts->tile_width, ts->tile_height));
						m_animatedBatchTexture.draw(tileSprite);
					}
				}
			}
		}
	}

	void TMXMap::draw_image_layer(tmx_image *img)
	{
		sf::Sprite s(*(sf::Texture*)img->resource_image);
		m_batchTexture.draw(s);
	}

	void TMXMap::render_map(tmx_map *map)
	{
		tmx_layer *layers = map->ly_head;
		
		m_batchTexture.clear(int_to_sf_colour(map->backgroundcolor));

		while (layers)
		{
			if (layers->visible)
			{
				if (layers->type == L_OBJGR)
				{
					draw_objects(layers->content.objgr);
				}
				else if (layers->type == L_IMAGE)
				{
					draw_image_layer(layers->content.image);
				}
				else if (layers->type == L_LAYER)
				{
					draw_layer(map, layers);
				}
			}

			layers = layers->next;
		}

		m_batchTexture.display();
	}

	void TMXMap::parseCollisions(tmx_map *map)
	{
		tmx_layer *layers = map->ly_head;

		while (layers)
		{
			std::string cln(layers->name);
			if (cln == "Collision")
			{
				tmx_object_group* objgr = layers->content.objgr;
				tmx_object *head = objgr->head;
				while (head)
				{
					sf::Rect<double> rect(head->x, head->y, head->width, head->height);
					m_tileCollisions.push_back(rect);

					head = head->next;
				}
			}
			
			layers = layers->next;
		}
	}

	TMXMap::TMXMap()
	{
		tmx_img_load_func = sf_tex_loader;
		tmx_img_free_func = sf_tex_deleter;
	}

	TMXMap::TMXMap(const std::string& map)
	{
		tmx_img_load_func = sf_tex_loader;
		tmx_img_free_func = sf_tex_deleter;

		m_map = tmx::make(map);
        RE_ASSERT(m_map.get(), tmx_strerr(), "TMXMap::TMXMap", "TMXMap.cpp", 302);

		m_width = m_map->width * m_map->tile_width;
		m_height = m_map->height * m_map->tile_height;

		m_batchTexture.create(m_width, m_height);
		m_animatedBatchTexture.create(m_width, m_height);

		render_map(m_map.get());
		parseCollisions(m_map.get());
	}

	TMXMap::~TMXMap()
	{
	}

	void TMXMap::load(const std::string& map)
	{
		m_map = tmx::make(map);
		RE_ASSERT(m_map.get(), tmx_strerr(), "TMXMap::load", "TMXMap.cpp", 321);

		m_width = m_map->width * m_map->tile_width;
		m_height = m_map->height * m_map->tile_height;

		m_animatedBatchTexture.create(m_width, m_height);
		m_batchTexture.create(m_width, m_height);

		render_map(m_map.get());
		parseCollisions(m_map.get());
	}

	void TMXMap::update(sf::Time dt)
	{
		tmx_layer* layers = m_map->ly_head;
		
		m_animatedBatchTexture.clear(sf::Color::Transparent);
		
		while (layers)
		{
			if (layers->visible) {
				if (layers->type == L_LAYER) {
					drawAnimationLayer(m_map.get(), layers, dt);
				}
			}
			layers = layers->next;
		}

		m_animatedBatchTexture.display();
	}

	std::vector<sf::Rect<double>>& TMXMap::getCollisions()
	{
		return m_tileCollisions;
	}

	void TMXMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		sf::Sprite batchA(m_batchTexture.getTexture());
		sf::Sprite batchB(m_animatedBatchTexture.getTexture());

		states.shader = nullptr;
		states.texture = nullptr;
		states.transform *= getTransform();

		target.draw(batchA, states);
		target.draw(batchB, states);
	}
}

/*
void PhysicsSystem::addMapCollisions(TMXMap* map, const std::string& mapCollisionConfigFile)
{
sol::state lua;
lua.script(Locator::get<VFS>()->toString(mapCollisionConfigFile));
sol::table mapConfig = lua.get<sol::table>("mapConfig");

for (auto& v : map->getCollisions())
{
b2BodyDef bodyDef;
bodyDef.position.Set((float)b2::pixelsToMeters<double>(v.left), (float)b2::pixelsToMeters<double>(v.top));

bodyDef.type = b2BodyType::b2_staticBody;

b2PolygonShape b2shape;
b2shape.SetAsBox((float32)b2::pixelsToMeters<double>(v.width / 2.0), (float32)b2::pixelsToMeters<double>(v.height / 2.0), b2Vec2((float32)b2::pixelsToMeters<double>(v.width / 2.0), (float32)b2::pixelsToMeters<double>(v.height / 2.0)), mapConfig.get<float32>("angle"));

b2FixtureDef fixtureDef;
fixtureDef.density = mapConfig.get<float32>("density");
fixtureDef.friction = mapConfig.get<float32>("friction");
fixtureDef.restitution = mapConfig.get<float32>("restitution");
fixtureDef.shape = &b2shape;

b2Body* body = m_manager->m_world->CreateBody(&bodyDef);
body->CreateFixture(&fixtureDef);
m_mapCollisions.push_back(body);
}
}
*/