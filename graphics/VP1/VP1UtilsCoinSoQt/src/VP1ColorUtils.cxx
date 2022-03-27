/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * author: Riccardo Maria BIANCHI <riccardo.maria.bianchi@cern.ch>, 2022 Jan
 */


#include "VP1UtilsCoinSoQt/VP1ColorUtils.h"

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/SbColor.h>

#include <iostream>
#include <stdlib.h>

float VP1ColorUtils::getValFromRGB(const unsigned int rgb)
{
    if (rgb > 255) {
        std::cout << "ERROR! RGB color value is expected in the the range [0,255]. Here we get: " << rgb << ". Exiting..." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    return rgb/255.0;
}

void VP1ColorUtils::setMatColorFromRGB(SoMaterial* mat, std::string type, const unsigned int r, const unsigned int g, const unsigned int b)
{
    const float fr = getValFromRGB(r);
    const float fg = getValFromRGB(g);
    const float fb = getValFromRGB(b);
    if (type == "ambient")
        mat->ambientColor.setValue(fr, fg, fb);
    else if (type == "diffuse")
        mat->diffuseColor.setValue(fr, fg, fb);
    else if (type == "specular")
        mat->specularColor.setValue(fr, fg, fb);
    else 
        std::cout << "ERROR! Color type not supported ==> " << type << std::endl;
    
    // Debug Msg
    //std::cout << "Set color (" << r << "," << g << "," << b << ") to (" << fr << "," << fg << "," << fb << ")" << std::endl;
    return;
}

SbColor VP1ColorUtils::getSbColorFromRGB(const unsigned int r, const unsigned int g, const unsigned int b)
{
    const float fr = getValFromRGB(r);
    const float fg = getValFromRGB(g);
    const float fb = getValFromRGB(b);
    return SbColor(fr, fg, fb);
}
    
