//
// Created by matf-rg on 30.10.20..
//

#ifndef PROJECT_BASE_TEXTURE2D_H
#define PROJECT_BASE_TEXTURE2D_H
#include <glad/glad.h>
#include <stb_image.h>
#include <rg/Error.h>

class Texture2D {
  unsigned int m_id;
  int width, height, nChanel;
  unsigned char* data;
  public:
    Texture2D(std::string imgPath, GLint wrap_param, GLint filter_param, GLint  format = 0){
        // generisemo tekstura ubjekat
        glGenTextures(1, &m_id);
        // postavimo ga kao GL_TEXTURE_2D
        glBindTexture(GL_TEXTURE_2D, m_id);

        // parametri koje treba postaviti: wrapovanje, filtriranje, potom load img
        // wrap
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_param);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_param);
        // filtriranje
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_param);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_param);

        stbi_set_flip_vertically_on_load(true);

        data = stbi_load(imgPath.c_str(), &width, &height, &nChanel, 0);
        if(data) {
            if (nChanel == 1) {
              format = GL_RED;
            }
            else if (nChanel == 2) {
              format = GL_RG;
            }
            else if (nChanel == 3) {
              format = GL_RGB;
            } else if (nChanel == 4) {
              format = GL_RGBA;
            }
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "Failed to load texture" << std::endl;
        }

        // sama ucitana slika za teksturu nam vise ne treba!
        stbi_image_free(data);
    }
    void bind(){
        glBindTexture(GL_TEXTURE_2D, m_id);
    }
};

#endif //PROJECT_BASE_TEXTURE2D_H
