#include "TextureAnimation.h"
#include "Texture.h"
#include "Framebuffer.h"

#include <GL\glew.h>
#include <unordered_set>

TextureAnimator::TextureAnimator() :
	frameBuffer(new Framebuffer(1u, 1u))
{
}

TextureAnimator::~TextureAnimator() {
	delete frameBuffer;
}

void TextureAnimator::addAnimations(const std::vector<TextureAnimation>& animations) {
	for (const auto& elem : animations) {
		addAnimation(elem);
	}
}

void TextureAnimator::update(float delta) {
	std::unordered_set<uint> changedTextures;

	frameBuffer->bind();
	for (auto& elem : animations) {
		if (changedTextures.find(elem.dstTexture->id) == changedTextures.end()) changedTextures.insert(elem.dstTexture->id);
		elem.timer -= delta;
		Frame& frame = elem.frames[elem.currentFrame];
		if (elem.timer <= 0) {
			elem.timer = frame.duration;
			elem.currentFrame++;
			if (elem.currentFrame >= elem.frames.size()) elem.currentFrame = 0;

			glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, elem.srcTexture->id, 0);
			glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, elem.dstTexture->id, 0);
			glDrawBuffer(GL_COLOR_ATTACHMENT1);

			float srcPosY = elem.srcTexture->height - frame.size.y - frame.srcPos.y; // vertical flip

			glBlitFramebuffer(frame.srcPos.x, srcPosY,			frame.srcPos.x + frame.size.x,	srcPosY + frame.size.y,
							  frame.dstPos.x, frame.dstPos.y,	frame.dstPos.x + frame.size.x,	frame.dstPos.y + frame.size.y,
							  GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}
	}
	for (auto& elem : changedTextures) {
		glBindTexture(GL_TEXTURE_2D, elem);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}
