#include "zxpch.h"

#include "FrameBuffer.h"
#include "Application.h"

FrameBuffer::FrameBuffer()
{
	auto window = Application::Get().GetActiveWindow();

	m_Width = window->GetWidth();
	m_Height = window->GetHeight();
	Invalidate();
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_FrameBufferId);
	glDeleteTextures(1, &m_ColorAttachmentId);
}

void FrameBuffer::Resize(unsigned int width, unsigned int height)
{
	m_Width = width;
	m_Height = height;

	Invalidate();
}

void FrameBuffer::Invalidate()
{
	if (m_FrameBufferId)
	{
		glDeleteFramebuffers(1, &m_FrameBufferId);
		glDeleteTextures(1, &m_ColorAttachmentId);	
	}

	glGenFramebuffers(1, &m_FrameBufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferId);

	glGenTextures(1, &m_ColorAttachmentId);
	glBindTexture(GL_TEXTURE_2D, m_ColorAttachmentId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachmentId, 0);
	
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::stringstream msg;
		msg << "Framebuffer is not complete! Status:" << status << std::endl;
		Application::Get().LogMessage(Severity::Error, msg.str());
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
