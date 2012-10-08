/*-
 * Copyright (c) 2012 Ilya Kaliman
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef LIBEFP_MATH_UTIL_H
#define LIBEFP_MATH_UTIL_H

#include <math.h>
#include <string.h>

#define PI 3.14159265358979323846
#define EPSILON 1.0e-8

#define VEC(x) ((vec_t *)(&(x)))
#define CVEC(x) ((const vec_t *)(&(x)))

typedef struct {
	double x, y, z;
} vec_t;

typedef struct {
	double x, y, z, a, b, c;
} six_t;

typedef struct {
	double xx, xy, xz, yx, yy, yz, zx, zy, zz;
} mat_t;

static const vec_t vec_zero = { 0.0, 0.0, 0.0 };

static const six_t six_zero = { 0.0, 0.0, 0.0,
				0.0, 0.0, 0.0 };

static const mat_t mat_zero = { 0.0, 0.0, 0.0,
				0.0, 0.0, 0.0,
				0.0, 0.0, 0.0 };

static inline int
eq(double a, double b)
{
	return fabs(a - b) < EPSILON;
}

static inline double
vec_get(const vec_t *vec, int idx)
{
	return ((const double *)vec)[idx];
}

static inline void
vec_negate(vec_t *vec)
{
	vec->x = -vec->x, vec->y = -vec->y, vec->z = -vec->z;
}

static inline double
vec_dot(const vec_t *a, const vec_t *b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

static inline vec_t
vec_cross(const vec_t *a, const vec_t *b)
{
	vec_t c = {
		a->y * b->z - a->z * b->y,
		a->z * b->x - a->x * b->z,
		a->x * b->y - a->y * b->x
	};
	return c;
}

static inline void
vec_atomic_add(vec_t *a, const vec_t *b)
{
	#pragma omp atomic
	a->x += b->x;
	#pragma omp atomic
	a->y += b->y;
	#pragma omp atomic
	a->z += b->z;
}

static inline void
vec_atomic_sub(vec_t *a, const vec_t *b)
{
	#pragma omp atomic
	a->x -= b->x;
	#pragma omp atomic
	a->y -= b->y;
	#pragma omp atomic
	a->z -= b->z;
}

static inline vec_t
vec_add(const vec_t *a, const vec_t *b)
{
	vec_t c = { a->x + b->x, a->y + b->y, a->z + b->z };
	return c;
}

static inline vec_t
vec_sub(const vec_t *a, const vec_t *b)
{
	vec_t c = { a->x - b->x, a->y - b->y, a->z - b->z };
	return c;
}

static inline double
vec_len_2(const vec_t *a)
{
	return vec_dot(a, a);
}

static inline double
vec_len(const vec_t *a)
{
	return sqrt(vec_len_2(a));
}

static inline void
vec_normalize(vec_t *vec)
{
	double len = vec_len(vec);

	vec->x /= len;
	vec->y /= len;
	vec->z /= len;
}

static inline double
vec_dist_2(const vec_t *a, const vec_t *b)
{
	vec_t dr = vec_sub(a, b);
	return vec_len_2(&dr);
}

static inline double
vec_dist(const vec_t *a, const vec_t *b)
{
	return sqrt(vec_dist_2(a, b));
}

static inline double
mat_get(const mat_t *mat, int a1, int a2)
{
	return ((const double *)mat)[3 * a1 + a2];
}

static inline void
mat_set(mat_t *mat, int a1, int a2, double val)
{
	((double *)mat)[3 * a1 + a2] = val;
}

static inline vec_t
mat_vec(const mat_t *mat, const vec_t *vec)
{
	vec_t out = { mat->xx * vec->x + mat->xy * vec->y + mat->xz * vec->z,
		      mat->yx * vec->x + mat->yy * vec->y + mat->yz * vec->z,
		      mat->zx * vec->x + mat->zy * vec->y + mat->zz * vec->z };
	return out;
}

static inline vec_t
mat_trans_vec(const mat_t *mat, const vec_t *vec)
{
	vec_t out = { mat->xx * vec->x + mat->yx * vec->y + mat->zx * vec->z,
		      mat->xy * vec->x + mat->yy * vec->y + mat->zy * vec->z,
		      mat->xz * vec->x + mat->yz * vec->y + mat->zz * vec->z };
	return out;
}

static inline mat_t
mat_mat(const mat_t *m1, const mat_t *m2)
{
	mat_t out = { m1->xx * m2->xx + m1->xy * m2->yx + m1->xz * m2->zx,
		      m1->xx * m2->xy + m1->xy * m2->yy + m1->xz * m2->zy,
		      m1->xx * m2->xz + m1->xy * m2->yz + m1->xz * m2->zz,
		      m1->yx * m2->xx + m1->yy * m2->yx + m1->yz * m2->zx,
		      m1->yx * m2->xy + m1->yy * m2->yy + m1->yz * m2->zy,
		      m1->yx * m2->xz + m1->yy * m2->yz + m1->yz * m2->zz,
		      m1->zx * m2->xx + m1->zy * m2->yx + m1->zz * m2->zx,
		      m1->zx * m2->xy + m1->zy * m2->yy + m1->zz * m2->zy,
		      m1->zx * m2->xz + m1->zy * m2->yz + m1->zz * m2->zz };
	return out;
}

static inline double
mat_det(const mat_t *mat)
{
	return mat->xx * mat->yy * mat->zz +
	       mat->xy * mat->zx * mat->yz +
	       mat->yx * mat->xz * mat->zy -
	       mat->xz * mat->yy * mat->zx -
	       mat->zy * mat->yz * mat->xx -
	       mat->yx * mat->xy * mat->zz;
}

static inline mat_t
mat_transpose(const mat_t *mat)
{
	mat_t out = { mat->xx, mat->yx, mat->zx,
		      mat->xy, mat->yy, mat->zy,
		      mat->xz, mat->yz, mat->zz };
	return out;
}

static inline mat_t
mat_inv(const mat_t *mat)
{
	double det = 1.0 / mat_det(mat);

	mat_t out = { det * (mat->yy * mat->zz - mat->yz * mat->zy),
		      det * (mat->zy * mat->xz - mat->zz * mat->xy),
		      det * (mat->xy * mat->yz - mat->xz * mat->yy),
		      det * (mat->yz * mat->zx - mat->yx * mat->zz),
		      det * (mat->zz * mat->xx - mat->zx * mat->xz),
		      det * (mat->xz * mat->yx - mat->xx * mat->yz),
		      det * (mat->yx * mat->zy - mat->yy * mat->zx),
		      det * (mat->zx * mat->xy - mat->zy * mat->xx),
		      det * (mat->xx * mat->yy - mat->xy * mat->yx) };

	return out;
}

#endif /* LIBEFP_MATH_UTIL_H */
