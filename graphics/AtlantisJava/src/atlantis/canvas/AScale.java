package atlantis.canvas;

import java.awt.geom.GeneralPath;

import atlantis.projection.AProjection2D;
import atlantis.projection.AProjection;

/**
 * This class contains static methods which are useful for the
 * AScaleBorder class and any potential subclasses.
 *
 * @author Charles Loomis, Ben Waugh
 **/
public class AScale {
	final GeneralPath primaryTicks;
	final GeneralPath secondaryTicks;
	final double[] labelValues;    // units are cm
	final double[] labelPositions; // after transform

	final static public int LEFT_TICKS=0;
	final static public int RIGHT_TICKS=1;
	final static public int BOTH_TICKS=2;

	/**
	 * The size in pixels of the primary tick marks.  This is a global
	 * property for all scales. */
	private static float primaryTickSize=8.f;

	/**
	 * The size in pixels of the secondary tick marks.  This is a
	 * global property for all scales. */
	private static float secondaryTickSize=5.f;

	private AScale(GeneralPath primaryTicks, GeneralPath secondaryTicks,
			double[] label, double[] position) {
		this.primaryTicks = primaryTicks;
		this.secondaryTicks = secondaryTicks;
		this.labelValues = label;
		this.labelPositions = position;
	}

	/**
	 * Set the tick sizes (in pixels) for the primary and secondary
	 * tick marks. */
	static public void setTickSizes(float primaryTickSize,
			float secondaryTickSize) {
		AScale.primaryTickSize=primaryTickSize;
		AScale.secondaryTickSize=secondaryTickSize;
	}

	/**
	 * Get the primary tick size (in pixels). */
	static public float getPrimaryTickSize() {
		return AScale.primaryTickSize;
	}

	/**
	 * Get the secondary tick size (in pixels). */
	static public float getSecondaryTickSize() {
		return AScale.secondaryTickSize;
	}

	/**
	 * A utility method to add a tick mark to the given GeneralPath at
	 * the given position. */
	static private void addTickMark(GeneralPath gp,
			int location,
			float tickPosition,
			float tickLength) {

		switch(location) {

		case(AScale.RIGHT_TICKS):
			gp.moveTo(tickPosition, 0.f);
		gp.lineTo(tickPosition, tickLength);
		break;

		case(AScale.LEFT_TICKS):
			gp.moveTo(tickPosition, 0.f);
		gp.lineTo(tickPosition, -tickLength);
		break;

		case(AScale.BOTH_TICKS):
			gp.moveTo(tickPosition, -tickLength);
		gp.lineTo(tickPosition, tickLength);
		break;
		}
	}

	/**
	 * Get scale with given parameters.
	 * 
	 * @param minValueTransformed min value after (fish-eye) transformation
	 * @param maxValueTransformed max value after (fish-eye) transformation
	 * @param scaleSize length of the scale border
	 * @param minPrimary minimum number of primary tick marks
	 * @param minSeparation minimum separation between tick marks
	 * @param location left,right,top,bottom
	 * @param isVertical whether this is a horizontal or vertical scale
	 * @param proj the projection this scale is drawn for
	 * @return
	 */
	public static AScale calculateScale(
			double minValueTransformed,
			double maxValueTransformed,
			int scaleSize,
			int minPrimary, 
			int minSeparation, 
			int location,
			int isVertical,
			AProjection proj
			){
		// Setup the two paths which will contain the primary and secondary tick marks.
		GeneralPath primaryTicks = new GeneralPath();
		GeneralPath secondaryTicks = new GeneralPath();
		resetScale(primaryTicks,scaleSize);
		resetScale(secondaryTicks,scaleSize);

		double minValue = invertTransformation(minValueTransformed,proj,isVertical);
		double maxValue = invertTransformation(maxValueTransformed,proj,isVertical);

		double[] labelValues;
		double[] labelPositions;
		double[] intervals = getScaleIntervals(minValue,maxValue);
		if (intervals!=null) {
			double labelInterval = intervals[0];
			double primarySpacing = intervals[1];
			double secondarySpacing = intervals[2];
			addTickMarks(primaryTicks, proj, minValue, maxValue, primarySpacing, isVertical, location, primaryTickSize, scaleSize);
			addTickMarks(secondaryTicks, proj, minValue, maxValue, secondarySpacing, isVertical, location, secondaryTickSize, scaleSize);

			// Now the labels with the correct precision must be made.
			labelValues = getScaleValues(minValue,maxValue,labelInterval);
			int numLabels = labelValues.length;
			labelPositions = new double[numLabels];
			int ndigits=(int)(Math.floor(Math.log10(primarySpacing)));
			ndigits=-Math.min(ndigits, 0);

			for (int i=0; i<labelValues.length; ++i) {
				double valueTrans = applyTransformation(labelValues[i],proj,isVertical);
				labelPositions[i] = (double) interpolate(valueTrans, scaleSize, minValueTransformed, maxValueTransformed);
			}
		} else {
			labelValues = new double[] {};
			labelPositions = new double[] {};
		}

		return new AScale(primaryTicks,secondaryTicks,labelValues,labelPositions);
	}
  
	/**
	 * Add tick marks at multiples of given spacing.
	 * 
	 * @param path
	 * @param proj
	 * @param min
	 * @param max
	 * @param spacing
	 * @param isVertical
	 * @param location 
	 * @param tickSize 
	 * @param scaleSize 
	 */
	private static void addTickMarks(GeneralPath path,
			AProjection proj, double min, double max, double spacing, int isVertical, int location, float tickSize, double scaleSize) {
		int minMultiplier = (int) Math.ceil(min/spacing);
		int maxMultiplier = (int) Math.floor(max/spacing);
		double minTrans = applyTransformation(min,proj,isVertical);
		double maxTrans = applyTransformation(max,proj,isVertical);
		for (int i=minMultiplier; i<=maxMultiplier; ++i) {
			double value = i * spacing;
			double valTrans = applyTransformation(value,proj,isVertical);
			float tickPosition=interpolate(valTrans, scaleSize, minTrans, maxTrans);        
			addTickMark(path, location, tickPosition, tickSize);
		}

	}

	private static void resetScale(GeneralPath path, double length) {
		path.reset();
		path.moveTo(0.f, 0.f);
		path.lineTo((float)length, 0.f);
	}

	/**
	 * Convert from physical detector coordinate to transformed (fish-eye) coordinate.
	 * 
	 * @param value
	 * @param proj
	 * @param isVertical
	 * @return
	 */
	private static double applyTransformation(double value, AProjection proj, int isVertical) {
		double valueTransformed = value;
		//Apply non-linear transforms if necessary
		if (proj instanceof AProjection2D) {
			if (isVertical==1) {
				valueTransformed = ((AProjection2D)proj).nonLinearTransform(0,value).getY();
			} else {
				valueTransformed = ((AProjection2D)proj).nonLinearTransform(value,0).getX();
			}
		}
		return valueTransformed;
	}

	/**
	 * Convert from transformed (fish-eye) coordinate back to physical detector coordinate.
	 * 
	 * @param valueTransformed
	 * @param proj
	 * @param isVertical
	 * @return
	 */
	private static double invertTransformation(double valueTransformed, AProjection proj, int isVertical) {
		double value = valueTransformed;
		//Apply non-linear transforms if necessary
		if (proj instanceof AProjection2D) {
			if (isVertical==1) {
				value = ((AProjection2D)proj).inverseNonLinearTransform(0,valueTransformed).getY();
			} else {
				value = ((AProjection2D)proj).inverseNonLinearTransform(valueTransformed,0).getX();
			}
		}
		return value;
	}

	static float interpolate(double value, double size,
			double value0, double value1) {
		return(float)(size*(value-value0)/(value1-value0));
	}

	/**
	 * Title is put between last two value labels on axis.
	 * @return the position for the title on the scale
	 */
	public double getTitlePosition() {
		double axisPosition;
		int numLabels = labelPositions.length;
		if (numLabels>=2) {
			axisPosition = 0.5 * (labelPositions[numLabels-1] + labelPositions[numLabels-2]);
		} else {
			axisPosition = 0.0; // default to 0 if only one label (shouldn't happen)
		}
		return axisPosition;
	}

	/**
	 * Get array of three intervals to use on scale.
	 * @param min minimum value on scale
	 * @param max maximum value on scale
	 * @return intervals to use for [labels, primary tick marks, secondary tick marks]
	 */
	static double[] getScaleIntervals(double min, double max) {
		if (min>=max) return null;
		boolean rangeIncludesZero = (min<0.0 && max>0.0);
		double labelInterval;
		double labelIntervalUnit;
		int labelIntervalMultiplier;
		if (rangeIncludesZero) {
			double maxAbsValue = Math.max(Math.abs(min), Math.abs(max));
			double m=Math.floor(Math.log10(maxAbsValue));
			labelIntervalUnit = Math.pow(10., m);
			labelInterval = getRoundNumber(maxAbsValue);
			labelIntervalMultiplier = (int) Math.round(labelInterval / labelIntervalUnit);
			if (labelIntervalMultiplier==2 && 2*labelInterval<maxAbsValue) {
				labelIntervalMultiplier = 4;
				labelInterval = labelIntervalMultiplier * labelIntervalUnit;
			}
		} else {
			double range = Math.abs(max-min);
			labelInterval = getRoundNumber(range);
			int minMultiplier = (int) Math.ceil(min/labelInterval);
			int maxMultiplier = (int) Math.floor(max/labelInterval);
			while (maxMultiplier==minMultiplier) {
				labelInterval = getRoundNumber(labelInterval*0.99);
				minMultiplier = (int) Math.ceil(min/labelInterval);
				maxMultiplier = (int) Math.floor(max/labelInterval);
			}
			double m=Math.floor(Math.log10(labelInterval));
			labelIntervalUnit = Math.pow(10., m);
			minMultiplier = (int) Math.ceil(min/labelInterval);
			maxMultiplier = (int) Math.floor(max/labelInterval);
			labelIntervalMultiplier = (int) (labelInterval/labelIntervalUnit);
		}

		double primaryInterval, secondaryInterval;
		switch (labelIntervalMultiplier) {
		case 1:
			primaryInterval = labelIntervalUnit * 0.5;
			secondaryInterval = labelIntervalUnit * 0.1;
			break;
		case 2:
			primaryInterval = labelIntervalUnit * 1.0;
			secondaryInterval = labelIntervalUnit * 0.2;
			break;
		case 3:
			primaryInterval = labelIntervalUnit * 0.2;
			secondaryInterval = labelIntervalUnit * 0.1;
			break;
		case 4:
			primaryInterval = labelIntervalUnit * 2.0;
			secondaryInterval = labelIntervalUnit * 0.5;
			break;
		case 5:
			primaryInterval = labelIntervalUnit * 1.0;
			secondaryInterval = labelIntervalUnit * 0.5;
			break;
		default: // Should not happen
			primaryInterval = labelIntervalUnit * 0.5;
			secondaryInterval = labelIntervalUnit * 0.1;
		}
		return new double[] {labelInterval, primaryInterval, secondaryInterval};
	}

	/**
	 * Get array of multiples of the given interval that lie in the given range.
	 * @param minValue minimum value on scale
	 * @param maxValue maximum value on scale
	 * @param labelInterval interval between values
	 * @return array of calculate values
	 */
	static double[] getScaleValues(double minValue, double maxValue,
			double interval) {
		int min = (int) Math.ceil(minValue/interval);
		int max = (int) Math.floor(maxValue/interval);
		int numValues = 1 + max - min;
		double[] values = new double[numValues];
		for (int i=0; i<numValues; ++i) {
			values[i] = (min + i) * interval;
		}
		return values;
	}

	/**
	 * Rounds number towards zero to nearest "round number", i.e. a power of ten
	 * multiplied by 1, 2 or 5.
	 * 
	 * @param d number to round
	 * @return next round number towards zero from input
	 */
	static double getRoundNumber(double d) {
		double magnitude = Math.abs(d);
		double m=Math.floor(Math.log10(magnitude));
		double unit = Math.pow(10., m);
		double multIn = magnitude / unit;
		double multOut;
		if (multIn >= 5.0) multOut = 5.0;
		else if (multIn >= 2.0) multOut = 2.0;
		else multOut = 1.0;
		double magRound = unit * multOut;
		double round;
		if (d<0.0) round = -magRound;
		else round = magRound;
		return round;
	}
}
