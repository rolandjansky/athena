/**
 * 
 */
package atlantis.utils;

import static org.junit.Assert.*;
import org.junit.Test;

import static java.lang.Math.PI;

/**
 * @author waugh
 *
 */
public class AMathTest {
	/** Arbitrary tolerance for floating-point comparisons */
	private static final double TOLERANCE = 0.00001;

	/**
	 * Test method for {@link atlantis.utils.AMath#nearestPhiDegrees(double)}.
	 */
	@Test
	public void testNearestPhiDegreesDouble() {
		assertEquals("Wrong value for phi",0.,AMath.nearestPhiDegrees(0.),TOLERANCE);
		assertEquals("Wrong value for phi",0.,AMath.nearestPhiDegrees(360.),TOLERANCE);
		assertEquals("Wrong value for phi",180.,AMath.nearestPhiDegrees(180.),TOLERANCE);
		assertEquals("Wrong value for phi",1.,AMath.nearestPhiDegrees(1.),TOLERANCE);
		assertEquals("Wrong value for phi",359.,AMath.nearestPhiDegrees(-1.),TOLERANCE);
	}

	/**
	 * Test method for {@link atlantis.utils.AMath#nearestPhiRadians(double)}.
	 */
	@Test
	public void testNearestPhiRadiansDouble() {
		assertEquals("Wrong value for phi",0.,AMath.nearestPhiRadians(0.),TOLERANCE);
		assertEquals("Wrong value for phi",0.,AMath.nearestPhiRadians(2*PI),TOLERANCE);
		assertEquals("Wrong value for phi",PI,AMath.nearestPhiRadians(PI),TOLERANCE);
		assertEquals("Wrong value for phi",0.1,AMath.nearestPhiRadians(0.1),TOLERANCE);
		assertEquals("Wrong value for phi",2*PI-0.1,AMath.nearestPhiRadians(-0.1),TOLERANCE);
	}

	/**
	 * Test method for {@link atlantis.utils.AMath#nearestPhiDegrees(double, double)}.
	 */
	@Test
	public void testNearestPhiDegreesDoubleDouble() {
		double phiMid = 360; // range [180,540)
		assertEquals("Wrong value for phi",180.,AMath.nearestPhiDegrees(180.0,phiMid),TOLERANCE);
		assertEquals("Wrong value for phi",180.,AMath.nearestPhiDegrees(540.0,phiMid),TOLERANCE);
		assertEquals("Wrong value for phi",360.,AMath.nearestPhiDegrees(360.0,phiMid),TOLERANCE);
		assertEquals("Wrong value for phi",361.,AMath.nearestPhiDegrees(1.0,phiMid),TOLERANCE);
		assertEquals("Wrong value for phi",539.,AMath.nearestPhiDegrees(179.0,phiMid),TOLERANCE);
	}

	/**
	 * Test method for {@link atlantis.utils.AMath#nearestPhiRadians(double, double)}.
	 */
	@Test
	public void testNearestPhiRadiansDoubleDouble() {
		double phiMid = 2*PI; // range [180,540)
		assertEquals("Wrong value for phi",PI,AMath.nearestPhiRadians(PI,phiMid),TOLERANCE);
		assertEquals("Wrong value for phi",PI,AMath.nearestPhiRadians(PI,phiMid),TOLERANCE);
		assertEquals("Wrong value for phi",2*PI,AMath.nearestPhiRadians(2*PI,phiMid),TOLERANCE);
		assertEquals("Wrong value for phi",2*PI+0.1,AMath.nearestPhiRadians(0.1,phiMid),TOLERANCE);
		assertEquals("Wrong value for phi",3*PI-0.1,AMath.nearestPhiRadians(PI-0.1,phiMid),TOLERANCE);
	}

}
