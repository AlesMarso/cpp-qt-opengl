#pragma once

#include <QPainter>

namespace render
{


	class IRender
	{
	public:
		IRender();
		virtual ~IRender();

		void Draw();
	};
}