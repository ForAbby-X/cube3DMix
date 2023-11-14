/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alde-fre <alde-fre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 17:45:10 by alde-fre          #+#    #+#             */
/*   Updated: 2023/11/14 17:46:53 by alde-fre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game.h"
#include "entity/entity.h"
#include "entity/all.h"
#include "particle/particle.h"
#include "aabb.h"
#include "gameplay_utils.h"

int	loop_la_vrai(t_engine *eng, t_data *game)
{
	if (game->state == 2)
		ft_put_text(eng, (t_v2i){3, eng->sel_spr->size[y] - 28},
			"[ESC] to free yourself...", 2);
	return (1);
}
