package atlantis.graphics;

import atlantis.graphics.colormap.AColorMap;

import java.awt.Color;

/**
 * Contains graphics attributes of a collection of data
 */
public class ADrawParameters {
	
	boolean draw;
	Color color;
	int size;
	int minSize=0;
	boolean forceSymbols=false;
	int lineWidth;
	int frameWidth;
	int symbol;
	int drawOrFill=AGraphics.DRAW;
	
	public ADrawParameters(boolean draw, int color, int size, int lineWidth, int frameWidth, int symbol) {
		Color[] colorMap=AColorMap.getColors();
		
		this.draw=draw;
		if(color!=-1)
			this.color=colorMap[color];
		else
			this.color=null;
		this.size=size;
		this.lineWidth=lineWidth;
		this.frameWidth=frameWidth;
		this.symbol=symbol;
	}
	
	public ADrawParameters(boolean draw, int color, int size, int lineWidth, int frameWidth, int symbol,
			boolean forceSymbols, int minSize, int drawOrFill) {
		Color[] colorMap=AColorMap.getColors();
		
		this.draw=draw;
		if(color!=-1)
			this.color=colorMap[color];
		else
			this.color=null;
		this.size=size;
		this.lineWidth=lineWidth;
		this.frameWidth=frameWidth;
		this.symbol=symbol;
		this.forceSymbols=forceSymbols;
		this.minSize=minSize;
		this.drawOrFill=drawOrFill;
	}
	
	public ADrawParameters(boolean draw, Color color, int size, int lineWidth, int frameWidth, int symbol, int drawOrFill) {
		this.draw=draw;
		this.color=color;
		this.size=size;
		this.lineWidth=lineWidth;
		this.frameWidth=frameWidth;
		this.symbol=symbol;
		this.drawOrFill=drawOrFill;
	}
	
	public void setColor(int color) {
		Color[] colorMap=AColorMap.getColors();
		
		this.color=colorMap[color];
	}
	
	public void setFrameWidth(int width) {
		this.frameWidth=width;
	}
	
	public void setSize(int size) {
		this.size=size;
	}
	
	public boolean toBeDrawn() {
		return draw;
	}
	
	public int getSize() {
		return size;
	}
	
	public int getMinSize() {
		return minSize;
	}
	
	public boolean getForceSymbols() {
		return forceSymbols;
	}
	
	public int getDrawOrFill() {
		return drawOrFill;
	}
	
}
