import java.awt.*; 
import javax.swing.*;
import java.io.File;


public class DataVisualizer
{


  public static void main(String args[])
  {
	int specID = 0;
	boolean processAllFiles = true;

	if (args.length != 0)
	{
		specID = Integer.parseInt(args[0]);
		processAllFiles = false;
	}
			
	System.out.println("DataVisualizer");

	// Allocating plot drawer
	PlotDrawer p = new PlotDrawer();

	String folderData = "../HVHistoryDat";
	String folderPict = "../HVHistoryPic";


	if ( ! processAllFiles)
	{
		// Load trip history from file
		Trip t = new Trip(specID, folderData + "/trip_" + specID + ".dat");
		
		// Draw corresponding plot
		p. drawHVPlot(t, folderPict + "/trip_" + specID + ".png");
	}
	else
	{
		// Looping all data files in directory
		File folder = new File(folderData);
		String[] fileNames = folder.list();

	
		if(fileNames != null)
		{
		  for (String filename : fileNames)
		  {
		    if(filename.endsWith(".dat") && filename.startsWith("trip_"))
		    {
			// Get trip id from filename
			int tripID = Integer.parseInt(filename.substring(5, filename.length() - 4)); // between "trip_" and ".dat" 
		
			System.out.println("Plot for trip ID = " + tripID + "\n");

			// Load trip history from file
			Trip t = new Trip(tripID, folderData + "/" + filename);
		
			// Draw corresponding plot
			p. drawHVPlot(t, folderPict + "/trip_" + tripID + ".png");
		    }
	    	  }
	  	}
	}

/*
	// For testing purpose only : displaying the generated plot

	Trip t = new Trip(1, "../HVHistoryDat/trip_1.dat");

	p. drawHVPlot(t, "../HVHistoryPic/trip_1.png");
	
	JFrame f = new JFrame("Trip info");
	f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	f.getContentPane().add(p);
	f.setLocation(200,100);
	f.pack();
	f.show();
*/
}








}