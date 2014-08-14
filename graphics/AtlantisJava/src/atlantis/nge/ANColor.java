package atlantis.nge;

import atlantis.graphics.colormap.AColorMap;

import java.awt.Color;

/**
 * A floating point colour class that enforces very little
 * 
 * @author Adam Davison, Mark Stockton
 */
public class ANColor {

    public float r;
    public float g;
    public float b;
    public float a;

    public ANColor(Color c) {
        r = ((float) c.getRed()) / 255.0f;
        g = ((float) c.getGreen()) / 255.0f;
        b = ((float) c.getBlue()) / 255.0f;
        a = ((float) c.getAlpha()) / 255.0f;
    }

    public ANColor(float _r, float _g, float _b, float _a) {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }

    public ANColor(float _r, float _g, float _b) {
        r = _r;
        g = _g;
        b = _b;
        a = 1.0f;
    }

    public static ANColor getBGColor() {
        return getMapColor(AColorMap.BG);
    }

    public static ANColor getMapColor(int index) {
        Color bg = AColorMap.getColors()[index];

        float r = ((float) bg.getRed()) / 255.0f;
        float g = ((float) bg.getGreen()) / 255.0f;
        float b = ((float) bg.getBlue()) / 255.0f;

        return new ANColor(r, g, b);
    }

    public static ANColor getMapColor(int index, float trans) {
        Color bg = AColorMap.getColors()[index];

        float r = ((float) bg.getRed()) / 255.0f;
        float g = ((float) bg.getGreen()) / 255.0f;
        float b = ((float) bg.getBlue()) / 255.0f;

        return new ANColor(r, g, b, trans);
    }
    
    public void mult(float c) {
        r *= c;
        g *= c;
        b *= c;
        if (r < 0.0f) { r = 0.0f; }
        if (r > 1.0f) { r = 1.0f; }
        if (g < 0.0f) { g = 0.0f; }
        if (g > 1.0f) { g = 1.0f; }
        if (b < 0.0f) { b = 0.0f; }
        if (b > 1.0f) { b = 1.0f; }
    }
    
    public void lighten(float c) {
        r = 1.0f - ((1.0f - r)*c);
        g = 1.0f - ((1.0f - g)*c);
        b = 1.0f - ((1.0f - b)*c);
        if (r < 0.0f) { r = 0.0f; }
        if (r > 1.0f) { r = 1.0f; }
        if (g < 0.0f) { g = 0.0f; }
        if (g > 1.0f) { g = 1.0f; }
        if (b < 0.0f) { b = 0.0f; }
        if (b > 1.0f) { b = 1.0f; }
    }
}
