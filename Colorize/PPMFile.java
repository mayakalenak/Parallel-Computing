
/**
 * This class provides a method to read a PPM file and modify the pixel values based on a condition.
 * It can also write out the modified file.
 * Written by Stephanie Taylor to allow us to us it in CS333 Project 8
 */
import java.io.*;

public class PPMFile {

    private Pixel[] pixels; // store the 2D grid as a flat array
    private int width; // width in pixels
    private int height; // height in pixels
    private int maxColorValue; // maximum value

    public void readFile(String inputFile) throws IOException {
        // Open the input file for reading
        FileInputStream fis = new FileInputStream(inputFile);
        BufferedReader br = new BufferedReader(new InputStreamReader(fis));
        try {
            // Read the PPM file header
            String magicNumber = br.readLine();
            String line = br.readLine(); // has width and height on same line
            String parts[] = line.split(" ");
            this.width = Integer.parseInt(parts[0]);
            this.height = Integer.parseInt(parts[1]);
            this.pixels = new Pixel[height * width];
            this.maxColorValue = Integer.parseInt(br.readLine());
        } finally {
            // Close the input and output files
            br.close();
        }

        fis = new FileInputStream(inputFile);
        DataInputStream di = new DataInputStream(fis);
        try {
            // find the 3 newlines, the first byte after that should be data
            int char_byte;
            int newline_count = 0;
            int char_counter = 0;
            for (char_counter = 0; char_counter < 100 && newline_count < 3; char_counter++) {
                // Look for 3 bytes with the value 0
                // they should be the newlines
                char_byte = di.readUnsignedByte();
                if ((char) char_byte == '\n') {
                    System.out.println("found one");
                    newline_count++;
                }
            }
            for (int idx = 0; idx < height * width; idx++) {
                // Read the RGB values of the pixel
                int red = di.readUnsignedByte();
                int green = di.readUnsignedByte();
                int blue = di.readUnsignedByte();
                // store them in the array
                this.pixels[idx] = new Pixel(red, green, blue);
            }
        } finally {
            // Close the input and output files
            di.close();
        }
    }

    // Change the R, G, B values of each Pixel to be the average
    // of the original
    public void grayscale(int numThreads) {
        Thread[] thread = new Thread[numThreads];

        for (int i = 0; i < numThreads; i++) {
            final int start = i * (pixels.length / numThreads);
            final int end = (i == numThreads - 1) ? pixels.length : (i + 1) * (pixels.length / numThreads);

            thread[i] = new Thread(() -> {
                for (int j = start; j < end; j++) {
                    Pixel pix = pixels[j];
                    int avg = (pix.r + pix.g + pix.b) / 3;
                    pix.r = avg;
                    pix.g = avg;
                    pix.b = avg;
                }
            });
            thread[i].start();
        }
        for (Thread t : thread) {
            try {
                t.join();}
            catch (InterruptedException e){
                e.printStackTrace();}
        }
    }

    // Write the contents to a PPM file named by outputFile
    public void writeFile(String outputFile) throws IOException {
        FileOutputStream fos = new FileOutputStream(outputFile);
        BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(fos));
        bw.write("P6");
        bw.newLine();
        bw.write(this.width + " " + this.height);
        bw.newLine();
        bw.write(Integer.toString(this.maxColorValue));
        System.out.println("max color " + this.maxColorValue);
        bw.newLine();
        bw.close();
        fos = new FileOutputStream(outputFile, true); // open in append mode
        DataOutputStream di = new DataOutputStream(fos);
        for (int idx = 0; idx < height * width; idx++) {
            // Write the RGB values of the pixel
            di.writeByte(pixels[idx].r);
            di.writeByte(pixels[idx].g);
            di.writeByte(pixels[idx].b);
        }
    }

    // Main method to run the PPMFile processing
    public static void main(String[] args) {
        PPMFile ppm = new PPMFile();
        String inputFile = "IMG_4203.ppm";
        String outputFile = "output.ppm";

        try {
            for (int threadI : new int[] { 1, 2, 4 }) {
                ppm.readFile(inputFile);
                long start = System.currentTimeMillis();
                ppm.grayscale(threadI);
                long stop = System.currentTimeMillis();
                System.out.println("PPM file processed successfully in " + (stop - start) + " milliseconds");
                ppm.writeFile(outputFile);
            }
        } catch (IOException e) {
            System.out.println("Error processing PPM file: " + e.getMessage());
        }
    }
}