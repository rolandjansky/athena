package atlantis.canvas;

import static org.junit.Assert.*;

import org.junit.Test;

public class AScaleBorderTest {

	@Test
	public void testGetNumDecimalPlaces() {
		assertEquals("Wrong number of decimal places",0,AScaleBorder.getNumDecimalPlaces(1.0));
		assertEquals("Wrong number of decimal places",0,AScaleBorder.getNumDecimalPlaces(10.0));
		assertEquals("Wrong number of decimal places",1,AScaleBorder.getNumDecimalPlaces(0.5));
		assertEquals("Wrong number of decimal places",2,AScaleBorder.getNumDecimalPlaces(0.02));
		assertEquals("Wrong number of decimal places",0,AScaleBorder.getNumDecimalPlaces(-2.0));
		assertEquals("Wrong number of decimal places",1,AScaleBorder.getNumDecimalPlaces(-0.5));
	}

}
