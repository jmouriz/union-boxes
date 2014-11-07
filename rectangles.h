#include "pseudo-oop.h"

enumeration (Side)
{
	LEFT,
	RIGHT
} Side;

class (Position)
{
	Value x;
	Value y;
} Position;

alias (Position, Point);

class (Size)
{
	Value width;
	Value height;
} Size;

class (Line)
{
	Point start;
	Point end;
} Line;

class (Rectangle)
{
	Position position;
	Size size;

	Point offset;

	Flag is_crossing;
	Flag is_moving;
} Rectangle;

class (Curve)
{
	Line from;
	Line to;
} Curve;

alias (Rectangle, Table);

class (Relation)
{
	Table table_a;
	Table table_b;
} Relation;
