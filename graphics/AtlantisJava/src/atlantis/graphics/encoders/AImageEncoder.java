package atlantis.graphics.encoders;

import atlantis.canvas.ACanvas;
import atlantis.utils.ALogger;

import java.awt.Dimension;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.RenderingHints;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.File;
import java.io.FileOutputStream;
import java.io.BufferedOutputStream;

import org.apache.batik.dom.svg.SVGDOMImplementation;
import org.apache.batik.svggen.SVGGraphics2D;

import org.sourceforge.jlibeps.epsgraphics.*;
import org.w3c.dom.*;
import org.w3c.dom.svg.SVGDocument;

/**
 * Takes the image drawn on the canvas and encodes it in the given format
 * @author sboeser
 */
public class AImageEncoder {

  //The logger
  private final static ALogger logger = ALogger.getLogger(AImageEncoder.class);

    /**
     * Grabs canvas and encodes the data in PNG format.
     * @param imageWidth resulting image width
     * @param imageHeight resulting image height
     * @return byte[]
     */
    public byte[] getPNGData(int imageWidth, int imageHeight){

      // Draw image
      Image image = new BufferedImage(imageWidth, imageHeight, BufferedImage.TYPE_INT_ARGB);
      ACanvas.getCanvas().drawScaledCanvas(image.getGraphics(), imageWidth, imageHeight, "png");

      // Encode as PNG.
      PngEncoder encoder = new PngEncoder(image, true, PngEncoder.FILTER_NONE, 9);
      byte[] data = encoder.pngEncode();
      return data;

    }

    /**
     * Get a scaled version of the PNG image - mostly used to create thumbnails
     * NOTE: in contrast to getPNGData, this will first create the image with given
     * width and height and then scale everything (including window
     * borders, title bar, etc...)
     * @param width image width before scaling
     * @param height image height before scaling
     * @param scale the scale factor
     * @return the PNG data
     */
    public byte[] getScaledPNGData(int width, int height, double scale){

      // Draw a temporary full-size image
      Image inputImage = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
      ACanvas.getCanvas().drawScaledCanvas(inputImage.getGraphics(), width, height, "png");

      //Calculate target height keeping aspect ratio
      int targetHeight = (int)(scale*height);
      int targetWidth = (int)(scale*width);


      /**
       * Now rescale the image in steps - do not use Image.getScaledInstance,
       * which is outdated and way to slow!
       */

      while (inputImage.getWidth(null) != targetWidth){

        //Resize by no more than a factor of two in one step
        if (inputImage.getWidth(null)/targetWidth > 2. ){
          width = (int)(inputImage.getWidth(null)/2.);
          height = (int)(inputImage.getHeight(null)/2.);
        } else {
          width = targetWidth;
          height = targetHeight;
        }

        //log for debugging
        logger.debug("Rescaling image from "+inputImage.getWidth(null)+"x"+inputImage.getHeight(null)+
                     " to "+width+"x"+height);

        //Now redraw that to the scaled image
        BufferedImage scaledImage = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
        Graphics2D scaledGraphics = scaledImage.createGraphics();
        scaledGraphics.setRenderingHint(RenderingHints.KEY_INTERPOLATION,RenderingHints.VALUE_INTERPOLATION_BICUBIC);
        scaledGraphics.drawImage(inputImage, 0, 0, width, height, null);
        scaledGraphics.dispose();

        //Provide the scaled image as new input in case we need to rescale once more
        inputImage = scaledImage;
      }

      // Encode as PNG.
      PngEncoder encoder = new PngEncoder(inputImage, true, PngEncoder.FILTER_NONE, 9);
      byte[] data = encoder.pngEncode();
      return data;

    }


    /**
     * Grab canvas, encode the information as EPS and save to a file
     * @param imageWidth image width
     * @param imageHeight  image height
     * @param saveFile save file
     */
    
    public void saveEPS(int imageWidth, int imageHeight, File saveFile) throws IOException{

      //Create the output stream
      FileOutputStream fos = new FileOutputStream(saveFile);
      BufferedOutputStream bos = new BufferedOutputStream(fos);

      // Create a new postscript graphics object
      EpsGraphics2D g = new EpsGraphics2D("Atlantis",bos,0,0,imageWidth,imageHeight);
      // Draw text as shapes
      g.setAccurateTextMode(true);
      // Draw into the postscript graphics
      ACanvas.getCanvas().drawScaledCanvas(g, imageWidth, imageHeight, "eps");
      g.flush();
      g.close();
      fos.close();
      bos.close();

    }
    
    /**
     * Grab canvas, encode the information as SVG and save to a file
     * @param imageWidth image width
     * @param imageHeight  image height
     * @param saveFile save file
     */
    
    public void saveSVG(int imageWidth, int imageHeight, File saveFile) throws IOException{

      // Document Object Model to define our SVG Document
      DOMImplementation domImpl =
          SVGDOMImplementation.getDOMImplementation();

      // We need to write the SVG into a Document
      String svgNS = SVGDOMImplementation.SVG_NAMESPACE_URI;
      SVGDocument doc = (SVGDocument) domImpl.createDocument(svgNS, "svg", null);
     
      // Construct an SVGGraphics2D from our Document
      SVGGraphics2D g = new SVGGraphics2D(doc);

      g.setSVGCanvasSize(new Dimension(imageWidth, imageHeight));

      // Paint the scaled Canvas onto our SVG Graphics2D
      ACanvas.getCanvas().drawScaledCanvas(g, imageWidth, imageHeight, "svg");

      // Stream the output into our chosen file
      g.stream(saveFile.getAbsolutePath());


    }
}
