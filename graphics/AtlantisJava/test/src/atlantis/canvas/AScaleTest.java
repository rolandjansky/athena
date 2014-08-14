package atlantis.canvas;

import static org.junit.Assert.*;

import java.awt.geom.GeneralPath;
import java.awt.geom.PathIterator;

import org.junit.Assert;
import org.junit.Test;

public class AScaleTest {
	
	private static final double TOLERANCE = 0.00001;

	@Test
	public void testCalculateScale() {
		AScale scale = AScale.calculateScale(0, 10, 100, 3, 7, 0, 0, null);
		for (double label : scale.labelValues) {
			System.out.println("label = "+label);
		}
		for (double pos : scale.labelPositions) {
			System.out.println("position: "+pos);
		}
		System.out.println("Primary tick marks = " + scale.primaryTicks);
		GeneralPath primaryTicks = scale.primaryTicks;
		PathIterator iterPTicks = primaryTicks.getPathIterator(null);
		double[] coords = new double[2];
		int i = 0;
		while (!iterPTicks.isDone()) {
			int type = iterPTicks.currentSegment(coords);
			System.out.format("point %d has type %d%n",i,type);
			System.out.print("Coords: ");
			for (int j=0; j<2; ++j) {
				System.out.format(" %f", coords[j]);
			}
			System.out.println();
			i++;
			iterPTicks.next();
		}
	}
	
	@Test
	public void testRoundNumbers() {
		assertEquals("Wrong rounding", 5.0, AScale.getRoundNumber(9.0), TOLERANCE);
		assertEquals("Wrong rounding", -5.0, AScale.getRoundNumber(-9.0), TOLERANCE);
		assertEquals("Wrong rounding", 1.0, AScale.getRoundNumber(1.01), TOLERANCE);
		assertEquals("Wrong rounding", 500.0, AScale.getRoundNumber(999.0), TOLERANCE);
	}
	
	@Test
	public void testGetScaleIntervals() {
		double[] intervals, expectedValues;
		intervals = AScale.getScaleIntervals(-11.0,11.0);
		expectedValues = new double[] {10.0, 5.0, 1.0};
		assertArrayEquals("Wrong intervals", expectedValues, intervals, TOLERANCE);
		intervals = AScale.getScaleIntervals(-110.0,110.0);
		expectedValues = new double[] {100.0, 50.0, 10.0};
		assertArrayEquals("Wrong intervals", expectedValues, intervals, TOLERANCE);
		intervals = AScale.getScaleIntervals(-51.0,51.0);
		expectedValues = new double[] {50.0, 10.0, 5.0};
		assertArrayEquals("Wrong intervals", expectedValues, intervals, TOLERANCE);
		intervals = AScale.getScaleIntervals(-50.0,50.0);
		expectedValues = new double[] {50.0, 10.0, 5.0};
		assertArrayEquals("Wrong intervals", expectedValues, intervals, TOLERANCE);
		intervals = AScale.getScaleIntervals(-49.0,49.0);
		expectedValues = new double[] {40.0, 20.0, 5.0};
		assertArrayEquals("Wrong intervals", expectedValues, intervals, TOLERANCE);
		intervals = AScale.getScaleIntervals(1.0,49.0);
		expectedValues = new double[] {20.0, 10.0, 2.0};
		assertArrayEquals("Wrong intervals", expectedValues, intervals, TOLERANCE);
		intervals = AScale.getScaleIntervals(1.0,101.0);
		expectedValues = new double[] {50.0, 10.0, 5.0};
		assertArrayEquals("Wrong intervals", expectedValues, intervals, TOLERANCE);
		intervals = AScale.getScaleIntervals(-101.0,-1.0);
		expectedValues = new double[] {50.0, 10.0, 5.0};
		assertArrayEquals("Wrong intervals", expectedValues, intervals, TOLERANCE);
		intervals = AScale.getScaleIntervals(-9.5,9.5);
		expectedValues = new double[] {5.0, 1.0, 0.5};
		assertArrayEquals("Wrong intervals", expectedValues, intervals, TOLERANCE);
		intervals = AScale.getScaleIntervals(19.0,61.0);
		expectedValues = new double[] {20.0, 10.0, 2.0};
		assertArrayEquals("Wrong intervals", expectedValues, intervals, TOLERANCE);
		intervals = AScale.getScaleIntervals(0.0,0.0);
		assertNull("Intervals should be null for invalid input",intervals);
	}
	
	@Test
	public void testGetScaleValues() {
		double[] values;
		double[] expectedValues;
		values = AScale.getScaleValues(0.0, 1.0, 1.0);
		expectedValues = new double[] {0.0, 1.0};
		assertArrayEquals("Wrong scale values", expectedValues, values, TOLERANCE);
		values = AScale.getScaleValues(-0.1, 1.1, 1.0);
		expectedValues = new double[] {0.0, 1.0};
		assertArrayEquals("Wrong scale values", expectedValues, values, TOLERANCE);
	}
	
	@Test
	public void testInterpolate() {
		float result = AScale.interpolate(0.5, 10.0, 0.0, 1.0);
		assertEquals("Wrong interpolated value",5.0,result,TOLERANCE);
	}
}
