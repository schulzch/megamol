#ifndef MOLSURFMAPCLUSTER_PICTUREDATA_INCLUDED
#define MOLSURFMAPCLUSTER_PICTUREDATA_INCLUDED

#include "image_calls/Image2DCall.h"
#include "vislib/graphics/gl/OpenGLTexture2D.h"
#include "vislib/graphics/BitmapImage.h"
#include "glowl/glowl.h"
#include <string>

namespace megamol {
namespace molsurfmapcluster {

    struct PictureData {
        std::string path;
        std::string pdbid;
        uint32_t width;
        uint32_t height;
        bool render;
        bool popup;
        //vislib::graphics::gl::OpenGLTexture2D* texture;
        std::unique_ptr<glowl::Texture2D> texture;
        vislib::graphics::BitmapImage* image;
        std::vector<float> valueImage;
    };

}
} // namespace megamol

#endif
