import java.util.LinkedList;
import java.util.Collections;
import java.util.Iterator;
import java.util.Date;
import java.text.SimpleDateFormat;

import java.awt.*; 
import java.awt.image.BufferedImage;
import java.awt.geom.AffineTransform;
import java.awt.Font;
import javax.swing.*;

import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;

public class PlotDrawer extends JPanel
{
   public static final int WIDTH = 900;
   public static final int HEIGHT = 600;
   public static final int BORDER = 50; 
   public static final int TYPE_SIZE = 8; 


   private LinkedList<HVNode>  data;
   private LinkedList<LumiBlock>  lumiblocks;

   private BufferedImage bi;	


   private long minTime, maxTime;
   private int minLB, maxLB;
   private float minVolt, maxVolt;
   private float minCur, maxCur;
   private int minStat, maxStat;
   private int hvline;
		
   public PlotDrawer()
   {
	data = new LinkedList<HVNode>();
	lumiblocks = new LinkedList<LumiBlock>();
   }   

   public void drawHVPlot(Trip t, String filename)
   {
	// Clearing previous data
	reset();

	hvline = t.HVLine;

	// Add smith to lists
	for (HVNode n : t.hvinfo)
        {
		data.add(n);

		minTime = Math.min(minTime, n.time);
		maxTime = Math.max(maxTime, n.time);
		minLB = Math.min(minLB, n.LB);
		maxLB = Math.max(maxLB, n.LB);
		minVolt = Math.min(minVolt, n.voltage);
		maxVolt = Math.max(maxVolt, n.voltage);
		minCur = Math.min(minCur, n.current);
		maxCur = Math.max(maxCur, n.current);
		minStat = Math.min(minStat, n.status);
		maxStat = Math.max(maxStat, n.status);

	}

	for (LumiBlock lb : t.LBs)
	{
		lumiblocks.add(lb);
		minTime = Math.min(minTime, lb.time);
		maxTime = Math.max(maxTime, lb.time);
	}

	// For a clear drawing
	minVolt = 0;
	
	Collections.sort(data);
	Collections.sort(lumiblocks);

	// Initialize 
	drawToFile(filename);

   }
   

   private void reset()
   {
	data.clear();
	minTime = Long.MAX_VALUE; maxTime = Long.MIN_VALUE;
  	minLB = Integer.MAX_VALUE; maxLB = Integer.MIN_VALUE;
 	minVolt = Float.MAX_VALUE; maxVolt = Float.MIN_VALUE;
	minCur = Float.MAX_VALUE; maxCur = Float.MIN_VALUE;
   	minStat = Integer.MAX_VALUE; maxStat = Integer.MIN_VALUE;
   } 	

   private void drawScale(Graphics2D g)
   {
	final int NPoints = 5;
        g.setColor(Color.BLUE);
	
	float dV = (maxVolt - minVolt) / NPoints;
	float dI = (maxCur - minCur) / NPoints;

	// Voltage axe

	g.drawString(" Voltage", 0, 2 * TYPE_SIZE);
 
	for (int i = 0; i <= NPoints; i++)
		g.drawString(String.format("%5.0f", minVolt + i * dV),
			     0,
		 	     HEIGHT - BORDER - (int)((i * dV) * (HEIGHT - 2 * BORDER) / (maxVolt - minVolt)));


	// Current axe
        g.setColor(Color.RED);

	g.drawString("Current ", WIDTH - BORDER, 2 * TYPE_SIZE);
	
	for (int i = 0; i <= NPoints; i++)
		g.drawString("   " + String.format("%4.2f", minCur + i * dI),
			     WIDTH - BORDER,
		 	     HEIGHT - BORDER - (int) (  (i * dI) * (HEIGHT - 2 * BORDER) / (maxCur - minCur)));

	g.setColor(Color.GREEN);
	g.drawString("Trip status", WIDTH/2 - BORDER, 2 * TYPE_SIZE);
	
	
	// Time axe	
	g.setColor(Color.GRAY);
	g.setFont(new Font("Arial", 1, 10));
	
	SimpleDateFormat df = new SimpleDateFormat("HH:mm:ss");

	for (LumiBlock lb : lumiblocks)
	{
		int x = BORDER + (int)((lb.time - minTime) * (WIDTH - 2 * BORDER) / (maxTime - minTime));
		g.drawLine(x, BORDER, x, HEIGHT-BORDER);
		g.drawString( "" + lb.LB, x - TYPE_SIZE, BORDER - 5);
		g.setTransform(AffineTransform.getRotateInstance(Math.PI / 6, x, HEIGHT - BORDER));
		g.drawString(df.format(new Date(lb.time*1000)), x, HEIGHT-BORDER+TYPE_SIZE);
		g.setTransform(new AffineTransform());
		

	}
	
	g.setColor(Color.BLACK);
	g.setFont(new Font("Arial", 1, 14));

	df = new SimpleDateFormat("dd-MM-yyyy");

	g.drawString(df.format(new Date(minTime*1000)), WIDTH - 2 * BORDER, HEIGHT - TYPE_SIZE);
	g.drawString("HVLINE: " + hvline, BORDER, HEIGHT - TYPE_SIZE);
	
   }

   private void drawAxes(Graphics2D g)
   {
        g.setColor(Color.WHITE);
        g.fillRect(0, 0, WIDTH, HEIGHT);
        g.setColor(Color.BLACK);
	g.drawLine(BORDER-TYPE_SIZE, HEIGHT-BORDER, WIDTH-BORDER+TYPE_SIZE, HEIGHT-BORDER);
        g.setColor(Color.BLUE);
	g.drawLine(BORDER-TYPE_SIZE, BORDER, BORDER-TYPE_SIZE, HEIGHT-BORDER);
        g.setColor(Color.RED);
	g.drawLine(WIDTH-BORDER+TYPE_SIZE, BORDER, WIDTH-BORDER+TYPE_SIZE, HEIGHT-BORDER);
   }	

   private void drawPlots(Graphics2D g)
   {
	int x1, y1, x2, y2;

	if (maxTime > minTime)
	{
		Iterator it = data.iterator(), itn = data.iterator();

		if(itn.hasNext())
			itn.next();

		HVNode n1, n2;
		
		while(itn.hasNext())
		{
			n1 = (HVNode)it.next();
			n2 = (HVNode)itn.next();
		
			x1 = BORDER + (int)((n1.time - minTime) * (WIDTH - 2 * BORDER) / (maxTime - minTime));
			x2 = BORDER + (int)((n2.time - minTime) * (WIDTH - 2 * BORDER) / (maxTime - minTime));
		
			if(maxStat > minStat)
			{
				g.setColor(Color.GREEN);
				y1 = HEIGHT - BORDER - (n1.status - minStat) * (HEIGHT - 2 * BORDER) / (maxStat - minStat);
				y2 = HEIGHT - BORDER - (n2.status - minStat) * (HEIGHT - 2 * BORDER) / (maxStat - minStat);
				g.fillOval(x1-2, y1-2, 4, 4);
				g.fillOval(x2-2, y2-2, 4, 4);
				g.drawLine(x1, y1, x2, y2);
			}

			if(maxCur > minCur)
			{
				g.setColor(Color.RED);
				y1 = HEIGHT - BORDER - (int)((n1.current - minCur) * (HEIGHT - 2 * BORDER) / (maxCur - minCur));
				y2 = HEIGHT - BORDER - (int)((n2.current - minCur) * (HEIGHT - 2 * BORDER) / (maxCur - minCur));
				g.fillOval(x1-2, y1-2, 4, 4);
				g.fillOval(x2-2, y2-2, 4, 4);
				g.drawLine(x1, y1, x2, y2);
			}

			if(maxVolt > minVolt)
			{
				g.setColor(Color.BLUE);
				y1 = HEIGHT - BORDER - (int)((n1.voltage - minVolt) * (HEIGHT - 2 * BORDER) / (maxVolt - minVolt));
				y2 = HEIGHT - BORDER - (int)((n2.voltage - minVolt) * (HEIGHT - 2 * BORDER) / (maxVolt - minVolt));
				g.fillOval(x1-2, y1-2, 4, 4);
				g.fillOval(x2-2, y2-2, 4, 4);
				g.drawLine(x1, y1, x2, y2);
			}
	
		}  
	}

   }


   private void drawToFile(String filename)
   {
	bi = new BufferedImage(WIDTH, HEIGHT, BufferedImage.TYPE_INT_ARGB);
	Graphics2D g = bi.createGraphics();
	g.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
	
	drawAxes(g);
	drawScale(g);
	drawPlots(g);

	// Save to file
	try
	{
		ImageIO.write(bi, "PNG", new File(filename));
	}
	catch (Exception e)
	{
		System.out.println("Error: " + e.getMessage());
	}
   }

   public void paint(Graphics g)
   {
	if (bi != null)
  		g.drawImage(bi, 0, 0, this);      
        
   }

}
