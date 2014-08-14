package atlantis.event;

import java.io.File;
import java.io.IOException;

import atlantis.utils.*;
import atlantis.canvas.ACanvas;
import atlantis.graphics.encoders.AImageEncoder;
import java.awt.Dimension;


/** 
 * Implements ANewEventListener and creates SVG files
 * upon each newEvent call. A scale factor allows to e.g. produce thumbnails by
 * scaling down image. The image producer works in two modes:
 * a) If no file name is given, a file name will be generated from run and event
 *    number. In case the output file exists, no file is created but a warning is written out.
 * b) If a file name is given, a temporary file is generated first, and then
 *    renamed to the given file name. If renaming fails, a warning is written out
 *    and the temporary file is deleted.
 * 
 * @author Tom McLaughlan
 */

public class ASvgImageProducer extends AImageProducer
{
	// the logger
	private static final ALogger logger = ALogger.getLogger(ASvgImageProducer.class);

	// Our image encode
	private final AImageEncoder imageEncoder = new AImageEncoder();

	/**
	 * Fully qualified constructor, 
	 * intializing all the attributes and check the parameters validity
	 * @param dir the directory in which to save all the files
	 * @param size the dimensions of the images
	 * @param scale the factor by which to scale the images
	 * @param fileName if not null, that file will be overwritten for each event
	 * @throws InstantiationException if we fail to create the object
	 */
	public ASvgImageProducer(String dir, Dimension size, double scale, String fileName) throws InstantiationException
	{
		super(dir, size, scale, fileName);
	}

	/**
	 * Called whenever a new event appears. Store an SVG in the directory,
	 * either with a generated file name  (no overwrite) or with a fixed file name (overwrite)
	 * @param event the event from which to generate the SVG
	 */
	public void newEvent(AEvent event)
	{

		//If auto had been set for size (height is negative), get height from Canvas
		if (imageSize.height < 0) imageSize.setSize(imageSize.width, ACanvas.getCanvas().getRespectiveHeight(imageSize.width));

		//Create the file to write the data into
		File outFile;
		//If we don't have a fixed file name,
		//generate a unique one based on run- and event number
		if (fixedFileName==null){

			//Now construct all the full path for the file we want to save
			String outFileName = String.format("%s%s%s.svg",directory,
					System.getProperty("file.separator"),
					getEventFileName(event));

			// Create file handles to the file and check if it exists
			outFile = new File(outFileName);
			//Do not overwrite any existing files
			if (outFile.exists()) {
				logger.warn("File "+ outFileName +" already exists - will not overwrite");
				return;
			}
			//otherwise generate a temporary file first,
			//then move it in place later
		} else {
			//make a temporary file in the final directory
			try {
				outFile = File.createTempFile(filePrefix+"_", ".svg",directory);
			} catch (IOException ex) {
				logger.warn("Failed to create temporary file in "+directory.getAbsolutePath());
				return;
			}
		}

		//Now try saving the acquired data,
		//and if it shall go to a fixed file, move it there
		try {
			imageEncoder.saveSVG(imageSize.width, imageSize.height, outFile);

			//move to fixed file name if requested
			if (fixedFileName != null){

				//Get a handle to the final destination
				File fixedFile = new File(directory,fixedFileName);

				//delete target if exits
				if (fixedFile.exists() && (!fixedFile.delete()))
					throw new IOException("Failed to delete existing file "+fixedFile.getAbsolutePath());

				//And move the new file in place
				if (!outFile.renameTo(fixedFile))
					throw new IOException("Failed to rename temporary file to "+fixedFile.getAbsolutePath());
			}
		} catch (IOException ioe) {
			//If we fail, throw a warning
			logger.warn("Could not save SVG files for history\n"+ioe.toString());
		} finally {
			//In any case, delete the pngFile, if it was just temporary
			if (fixedFileName != null) outFile.delete();
		}


	} 	
}
