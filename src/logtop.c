/*
 * Copyright (c) 2010 Julien Palard.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include "logtop.h"

struct logtop *new_logtop(size_t history_size)
{
    struct logtop *this;

    this = malloc(sizeof(*this));
    if (this == NULL)
        return NULL;
    this->history_start = 0;
    this->history_size = history_size;
    this->strings = NULL;
    this->history = new_history(this);
    this->top = new_avl(this);
    return this;
}

void delete_logtop(struct logtop *this)
{
    delete_history(this);
    delete_avl(this);
    free(this);
}

void logtop_feed(struct logtop *this, char *string)
{
    log_line_t *element;

    element = avl_get(this, string);
    avl_increment(this, element);
    history_update(this, element);
}

static void logtop_get_fill(void *data, int index, void *display_data)
{
    ((log_line_t **)display_data)[index - 1] = (log_line_t *)data;
}

log_line_t **logtop_get(struct logtop *this, size_t qte)
{
    log_line_t **items;

    items = calloc(qte + 1, sizeof(*items));
    avl_traverse(this, qte, logtop_get_fill, items);
    return items;
}

double logtop_timespan(struct logtop *this)
{
    history_element_t   *oldest_element;
    history_element_t   *newest_element;

    oldest_element = history_oldest_element(this);
    newest_element = history_newest_element(this);
    if (oldest_element != NULL && newest_element != NULL)
        return difftime(newest_element->time, oldest_element->time);
    else
        return 0;
}

unsigned int logtop_qte_of_elements(struct logtop *this)
{
    return history_length(this);
}
