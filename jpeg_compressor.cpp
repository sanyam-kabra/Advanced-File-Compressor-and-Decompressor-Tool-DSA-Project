#include <bits/stdc++.h>
#include <jpeglib.h>

class JPEGCompressor {
public:
    JPEGCompressor(const std::string& inputFile, const std::string& outputFile, int quality)
        : inputFile(inputFile), outputFile(outputFile), quality(quality) {}

    void compress() {
        // Open the input file
        FILE* infile = fopen(inputFile.c_str(), "rb");
        if (!infile) {
            throw std::runtime_error("Can't open input file: " + inputFile);
        }

        // Create and initialize the JPEG decompressor
        jpeg_decompress_struct cinfo;
        jpeg_error_mgr jerr;
        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_decompress(&cinfo);

        // Specify the data source for decompression
        jpeg_stdio_src(&cinfo, infile);
        jpeg_read_header(&cinfo, TRUE);
        jpeg_start_decompress(&cinfo);

        // Allocate memory for the image
        std::vector<unsigned char> buffer(cinfo.output_width * cinfo.output_height * cinfo.output_components);
        while (cinfo.output_scanline < cinfo.output_height) {
            unsigned char* row_pointer = buffer.data() + cinfo.output_scanline * cinfo.output_width * cinfo.output_components;
            jpeg_read_scanlines(&cinfo, &row_pointer, 1);
        }

        // Clean up the decompressor
        jpeg_finish_decompress(&cinfo);
        fclose(infile);

        // Create and initialize the JPEG compressor
        jpeg_compress_struct cinfo_out;
        jpeg_error_mgr jerr_out;
        cinfo_out.err = jpeg_std_error(&jerr_out);
        jpeg_create_compress(&cinfo_out);

        // Open the output file
        FILE* outfile = fopen(outputFile.c_str(), "wb");
        if (!outfile) {
            throw std::runtime_error("Can't open output file: " + outputFile);
        }
        jpeg_stdio_dest(&cinfo_out, outfile);

        // Set compression parameters
        cinfo_out.image_width = cinfo.output_width;
        cinfo_out.image_height = cinfo.output_height;
        cinfo_out.input_components = cinfo.output_components;
        cinfo_out.in_color_space = cinfo.out_color_space;

        jpeg_set_defaults(&cinfo_out);
        jpeg_set_quality(&cinfo_out, quality, TRUE);

        // Start compression
        jpeg_start_compress(&cinfo_out, TRUE);
        while (cinfo_out.next_scanline < cinfo_out.image_height) {
            unsigned char* row_pointer = buffer.data() + cinfo_out.next_scanline * cinfo_out.image_width * cinfo_out.input_components;
            jpeg_write_scanlines(&cinfo_out, &row_pointer, 1);
        }

        // Finish compression
        jpeg_finish_compress(&cinfo_out);
        fclose(outfile);

        // Clean up
        jpeg_destroy_compress(&cinfo_out);
    }

private:
    std::string inputFile;
    std::string outputFile;
    int quality;
};

int main() {
    std::string input_filename, output_filename;
    int quality;

    std::cout << "Enter input file path: ";
    std::cin >> input_filename;
    std::cout << "Enter output file path: ";
    std::cin >> output_filename;
    std::cout << "Enter compression quality (1-100): ";
    std::cin >> quality;

    try {
        JPEGCompressor compressor(input_filename, output_filename, quality);
        compressor.compress();
        std::cout << "Image compressed successfully.\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
