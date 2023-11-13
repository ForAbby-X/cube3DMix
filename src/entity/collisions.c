/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collisions.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alde-fre <alde-fre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/08 20:47:41 by alde-fre          #+#    #+#             */
/*   Updated: 2023/11/13 01:05:15 by alde-fre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "aabb.h"
#include "cast_ray.h"
#include "entity/entity.h"

int	is_aabb_in_aabb(t_aabb const box1, t_aabb const box2)
{
	t_v3f const	min1 = box1.pos;
	t_v3f const	max1 = box1.pos + box1.dim;
	t_v3f const	min2 = box2.pos;
	t_v3f const	max2 = box2.pos + box2.dim;

	if (min1[x] >= max2[x] || min2[x] >= max1[x])
		return (0);
	if (min1[y] >= max2[y] || min2[y] >= max1[y])
		return (0);
	if (min1[z] >= max2[z] || min2[z] >= max1[z])
		return (0);
	return (1);
}

static inline t_v3f	__vec_normal(
						t_v3f const pos,
						t_aabb const box,
						t_v3f const vel)
{
	t_v3f const	center = box.pos + box.dim / 2.f;
	t_v3f const	diff = (pos - center) / (box.dim / 2.f);
	t_v3f const	abs = {fabsf(diff[x]), fabsf(diff[y]), fabsf(diff[z])};
	t_v3f		norm;

	norm = (t_v3f){0.0f, 0.0f, 0.0f};
	if (abs[x] >= abs[y] && abs[x] >= abs[z])
		norm[x] = vel[x];
	if (abs[y] >= abs[x] && abs[y] >= abs[z])
		norm[y] = vel[y];
	if (abs[z] >= abs[x] && abs[z] >= abs[y])
		norm[z] = vel[z];
	return (norm);
}

void	collision_ent_inside(
			t_aabb const *const box1,
			t_v3f *const vel1,
			t_aabb const *const box2,
			t_v3f *const vel2)
{
	t_v3f const		center1 = box1->pos + box1->dim / 2.0f;
	t_v3f const		center2 = box2->pos + box2->dim / 2.0f;
	t_v3f			diff;
	t_v3f			off;
//TODO: fix this
	diff = center1 - center2;
	// diff[x] = fabsf(diff[x]);
	// diff[y] = fabsf(diff[y]);
	// diff[z] = fabsf(diff[z]);
	off = diff - (box1->dim + box2->dim) / 2.0f;
	off[y] = 0.0f;
	if (box1->type == AABB_IMMOVABLE && box2->type == AABB_MOVABLE)
		*vel2 += off;
	else if (box1->type == AABB_MOVABLE && box2->type == AABB_IMMOVABLE)
		*vel1 -= off;
	else if (box1->type == AABB_MOVABLE && box2->type == AABB_MOVABLE)
	{
		off /= 2.f;
		*vel1 -= off;
		*vel2 += off;
	}
}

int	aabb_solve(
		t_aabb const *const box1,
		t_v3f *const vel1,
		t_aabb const *const box2,
		t_v3f *const vel2)
{
	t_aabb const	box = {box2->pos - box1->dim / 2.f,
		box1->dim + box2->dim, 0};
	t_v3f const		center = box1->pos + box1->dim / 2.0f;
	float			t;
	t_v3f			off;

	// printf("collision_ent_outside\n");
	if ((*vel1)[x] == 0.f && (*vel1)[y] == 0.f && (*vel1)[z] == 0.f)
		return (0);
	if (ray_box_intersection(center, *vel1, box, &t) == 0)
		return (0);
	off = __vec_normal(center + *vel1 * t, box, *vel1)
		* (1.f - t);
	if (box1->type == AABB_IMMOVABLE && box2->type == AABB_MOVABLE)
		*vel2 += off;
	else if (box1->type == AABB_MOVABLE && box2->type == AABB_IMMOVABLE)
		*vel1 -= off;
	else if (box1->type == AABB_MOVABLE && box2->type == AABB_MOVABLE)
	{
		off /= 2.f;
		*vel1 -= off;
		*vel2 += off;
	}
	return (1);
}

static t_v3i const g_off_pos[] = {
{-1, 0, 0}, {1, 0, 0},
{0, -1, 0}, {0, 1, 0},
{0, 0, -1}, {0, 0, 1},
{-1, -1, 0}, {-1, 1, 0},
{1, -1, 0}, {1, 1, 0},
{-1, 0, -1}, {-1, 0, 1},
{1, 0, -1}, {1, 0, 1},
{0, -1, -1}, {0, -1, 1},
{0, 1, -1}, {0, 1, 1},
{-1, -1, -1}, {-1, -1, 1},
{-1, 1, -1}, {-1, 1, 1},
{1, -1, -1}, {1, -1, 1},
{1, 1, -1}, {1, 1, 1}
};

static inline int	__block_collision(
		t_map *const map,
		t_aabb *const box,
		t_v3f *const vel)
{
	t_v3i const		player_pos = v3ftoi(box->pos + box->dim / 2.f);
	int				index;
	t_v3i			block;
	t_aabb			block_box;
	int				ret;

	ret = 0;
	index = -1;
	while (++index < 26)
	{
		block = player_pos + g_off_pos[index];
		if (map_get(map, block) == cell_air)
			continue ;
		block_box = (t_aabb){v3itof(block), {1.0f, 1.0f, 1.0f}, 2};
		ret += aabb_solve(box, vel, &block_box, &(t_v3f){0});
	}
	return (ret);
}

static inline void	__ent_loop(
						t_vector *const entities,
						t_entity *const self)
{
	t_entity		*ent;
	t_length		index;

	ent = entities->data;
	index = 0;
	while (index < entities->size)
	{
		if (self != ent && ent->aabb.type != AABB_NONE
			&& !((self->type == ENTITY_PLAYER && ent->type == ENTITY_FIREBALL)
				|| (ent->type == ENTITY_PLAYER
					&& self->type == ENTITY_FIREBALL)))
		{
			if (is_aabb_in_aabb(self->aabb, ent->aabb))
				collision_ent_inside(&self->aabb, &self->vel, &ent->aabb, &ent->vel);
			else
			if (aabb_solve(&self->aabb, &self->vel, &ent->aabb, &ent->vel)
				&& self->collided == ENTITY_NONE)
			{
				self->collided = ent->type;
				ent->collided = self->type;
			}
		}
		++ent;
		++index;
	}
}

void	collision_ent(
			t_data *const game,
			t_vector *const entities)
{
	t_entity	*ent;
	t_length	index;

	ent = entities->data;
	index = -1;
	while (++index < entities->size)
	{
		ent->collided = ENTITY_NONE;
		if (ent->aabb.type != AABB_NONE)
			__ent_loop(entities, ent);
		++ent;
	}
	ent = entities->data;
	index = -1;
	while (++index < entities->size)
	{
		if (ent->type != ENTITY_ENNEMY_FISH
			&& __block_collision(&game->map, &ent->aabb, &ent->vel)
				&& ent->collided == ENTITY_NONE)
				ent->collided = ENTITY_GENERIC;
		ent->aabb.pos += ent->vel;
		ent->vel = (t_v3f){0};
		++ent;
	}
}
