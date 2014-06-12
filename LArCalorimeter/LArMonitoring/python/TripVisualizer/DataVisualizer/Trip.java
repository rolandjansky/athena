import java.util.List;
import java.util.LinkedList;
import java.util.StringTokenizer;
import java.text.ParsePosition;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.io.*;
import java.io.BufferedReader;
import java.io.FileReader;

public class Trip
{
  int tripID;
  int HVLine;
  List<HVNode> hvinfo;
  List<LumiBlock> LBs;

  public Trip(int id)
  {
	tripID = id;
	HVLine = 0;
	hvinfo = new LinkedList<HVNode>();
	LBs = new LinkedList<LumiBlock>();
  }

  public Trip(int id, String filename)
  {
	tripID = id;
	hvinfo = new LinkedList<HVNode>();
	LBs = new LinkedList<LumiBlock>();
	
	// Reading from file
	String str;
	StringTokenizer st;
	SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd:HH:mm:ss");

	try
	{
		BufferedReader in = new BufferedReader(new FileReader(filename));
		// Get HVLine number
		if ((str = in.readLine()) != null)
			HVLine = Integer.parseInt(str.substring(8, str.length()));
		
		// Get parameters varaiation
		while (((str = in.readLine()) != null) && !str.startsWith("LumiBlocks:")) 
		{
			st = new StringTokenizer(str);
      			long time = df.parse(st.nextToken(), new ParsePosition(0)).getTime()/1000;	// seconds from Jan, 1, 1970 
  			int lb = Integer.parseInt(st.nextToken());
     			float voltage = Float.parseFloat(st.nextToken());
      			float current = Float.parseFloat(st.nextToken());
     			int status = Integer.parseInt(st.nextToken());

			hvinfo.add(new HVNode(time, lb, voltage, current, status));
    	 	}

		while ((str = in.readLine()) != null)
		{
			st = new StringTokenizer(str);
 			int lb = Integer.parseInt(st.nextToken());
      			long time = df.parse(st.nextToken(), new ParsePosition(0)).getTime()/1000;	// seconds from Jan, 1, 1970 
 			LBs.add(new LumiBlock(lb, time));
 
		}
       		
    		in.close();
	}
	catch (IOException e)
	{
		System.out.println("Error: " + e.getMessage());
	}
  }

}