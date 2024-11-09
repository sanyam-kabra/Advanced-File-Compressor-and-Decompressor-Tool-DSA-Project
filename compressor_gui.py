import tkinter as tk
from tkinter import ttk
from tkinter import filedialog
from tkinter import messagebox
import subprocess
import os
import sys
import threading
class CompressorGUI:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("File Compressor")
        self.root.geometry("400x300")
        
        if getattr(sys, 'frozen', False):
            self.application_path = os.path.dirname(sys.executable)
        else:
            self.application_path = os.path.dirname(os.path.abspath(__file__))
            
        self.setup_main_window()
        
    def setup_main_window(self):
        # Clear any existing widgets
        for widget in self.root.winfo_children():
            widget.destroy()
            
        # Main window styling
        self.root.configure(bg='#f0f0f0')
        
        # Header
        header = tk.Label(
            self.root,
            text="What type of file do you want to compress/decompress?",
            font=("Arial", 12, "bold"),
            wraplength=350,
            bg='#f0f0f0',
            pady=20
        )
        header.pack()
        
        # Buttons frame
        button_frame = tk.Frame(self.root, bg='#f0f0f0')
        button_frame.pack(expand=True)
        
        # Text button
        text_btn = ttk.Button(
            button_frame,
            text="Text File",
            command=self.show_text_window,
            width=20
        )
        text_btn.pack(pady=10)
        
        # JPEG button
        jpeg_btn = ttk.Button(
            button_frame,
            text="JPEG File",
            command=self.show_jpeg_window,
            width=20
        )
        jpeg_btn.pack(pady=10)
        
    def show_text_window(self):
        # Clear current window
        for widget in self.root.winfo_children():
            widget.destroy()

        # Header
        header = tk.Label(
            self.root,
            text="Text File Compression/Decompression",
            font=("Arial", 12, "bold"),
            bg='#f0f0f0',
            pady=20
        )
        header.pack()

        # Operation selection
        op_frame = tk.Frame(self.root, bg='#f0f0f0')
        op_frame.pack(fill='x', padx=20)

        operation = tk.StringVar(value="compress")

        compress_radio = ttk.Radiobutton(
            op_frame,
            text="Compress",
            variable=operation,
            value="compress"
        )
        compress_radio.pack(side='left', padx=20)

        decompress_radio = ttk.Radiobutton(
            op_frame,
            text="Decompress",
            variable=operation,
            value="decompress"
        )
        decompress_radio.pack(side='left', padx=20)

        # Input file selection
        input_frame = tk.Frame(self.root, bg='#f0f0f0')
        input_frame.pack(fill='x', padx=20, pady=10)

        input_path = tk.StringVar()
        input_entry = ttk.Entry(input_frame, textvariable=input_path, width=30)
        input_entry.pack(side='left', padx=5)

        input_btn = ttk.Button(
            input_frame,
            text="Browse Input",
            command=lambda: input_path.set(filedialog.askopenfilename())
        )
        input_btn.pack(side='left', padx=5)

        # Output file selection
        output_frame = tk.Frame(self.root, bg='#f0f0f0')
        output_frame.pack(fill='x', padx=20, pady=10)

        output_path = tk.StringVar()
        output_entry = ttk.Entry(output_frame, textvariable=output_path, width=30)
        output_entry.pack(side='left', padx=5)

        output_btn = ttk.Button(
            output_frame,
            text="Browse Output",
            command=lambda: output_path.set(filedialog.asksaveasfilename())
        )
        output_btn.pack(side='left', padx=5)

        # Process button
        process_btn = ttk.Button(
            self.root,
            text="Start Process",
            command=lambda: self.process_text(operation.get(), input_path.get(), output_path.get())
        )
        process_btn.pack(pady=20)

        # Home button
        home_btn = ttk.Button(
            self.root,
            text="Back to Home",
            command=self.setup_main_window
        )
        home_btn.pack(pady=10)
        
    def show_jpeg_window(self):
        # Clear current window
        for widget in self.root.winfo_children():
            widget.destroy()
            
        # Header
        header = tk.Label(
            self.root,
            text="JPEG File Compression",
            font=("Arial", 12, "bold"),
            bg='#f0f0f0',
            pady=20
        )
        header.pack()
        
        # Input file selection
        input_frame = tk.Frame(self.root, bg='#f0f0f0')
        input_frame.pack(fill='x', padx=20, pady=10)
        
        input_path = tk.StringVar()
        input_entry = ttk.Entry(input_frame, textvariable=input_path, width=30)
        input_entry.pack(side='left', padx=5)
        
        input_btn = ttk.Button(
            input_frame,
            text="Browse Input",
            command=lambda: input_path.set(filedialog.askopenfilename(filetypes=[("JPEG files", "*.jpg *.jpeg")]))
        )
        input_btn.pack(side='left', padx=5)
        
        # Output file selection
        output_frame = tk.Frame(self.root, bg='#f0f0f0')
        output_frame.pack(fill='x', padx=20, pady=10)
        
        output_path = tk.StringVar()
        output_entry = ttk.Entry(output_frame, textvariable=output_path, width=30)
        output_entry.pack(side='left', padx=5)
        
        output_btn = ttk.Button(
            output_frame,
            text="Browse Output",
            command=lambda: output_path.set(filedialog.asksaveasfilename(defaultextension=".jpg"))
        )
        output_btn.pack(side='left', padx=5)
        
        # Quality selection
        quality_frame = tk.Frame(self.root, bg='#f0f0f0')
        quality_frame.pack(fill='x', padx=20, pady=10)
        
        quality_label = tk.Label(
            quality_frame,
            text="Compression Quality (1-100):",
            bg='#f0f0f0'
        )
        quality_label.pack(side='left', padx=5)
        
        quality_var = tk.StringVar(value="75")
        quality_entry = ttk.Entry(quality_frame, textvariable=quality_var, width=5)
        quality_entry.pack(side='left', padx=5)
        
        # Process button
        process_btn = ttk.Button(
            self.root,
            text="Start Compression",
            command=lambda: self.process_jpeg(input_path.get(), output_path.get(), quality_var.get())
        )
        process_btn.pack(pady=20)
        
        # Home button
        home_btn = ttk.Button(
            self.root,
            text="Back to Home",
            command=self.setup_main_window
        )
        home_btn.pack(pady=10)
        
    def show_success_window(self, message):
        # Clear current window
        for widget in self.root.winfo_children():
            widget.destroy()
            
        # Success message
        success_label = tk.Label(
            self.root,
            text=message,
            font=("Arial", 12, "bold"),
            bg='#f0f0f0',
            pady=40
        )
        success_label.pack(expand=True)
        
        # Home button
        home_btn = ttk.Button(
            self.root,
            text="Back to Home",
            command=self.setup_main_window
        )
        home_btn.pack(pady=20)

    def process_text(self, operation, input_path, output_path):
        if not input_path or not output_path:
            messagebox.showerror("Error", "Please select both input and output files")
            return
            
        try:
            # Prepare arguments for the C++ executable
            args = [
                os.path.join(self.application_path, "compressor.exe"),
                "text",
                operation,
                input_path,
                output_path
            ]
            
            # Run the compressor
            self.run_compressor(args)
            
            # Show success message
            if operation == "compress":
                self.show_success_window("Text file compression completed successfully!")
            else:
                self.show_success_window("Text file decompression completed successfully!")
        except Exception as e:
            messagebox.showerror("Error", f"An error occurred: {str(e)}")

    def process_jpeg(self, input_path, output_path, quality):
        if not input_path or not output_path:
            messagebox.showerror("Error", "Please select both input and output files")
            return

        quality = int(quality)
        if quality < 1 or quality > 100:
            messagebox.showerror("Error", "Quality must be between 1 and 100")
            return

        def run_jpeg_compression():
            try:
                args = [
                    os.path.join(self.application_path, "compressor.exe"),
                    "jpeg",
                    "compress",
                    input_path,
                    output_path,
                    str(quality)
                ]
                stdout = self.run_compressor(args)
                self.show_success_window(stdout.strip())
            except Exception as e:
                messagebox.showerror("Error", f"An error occurred: {str(e)}")

        threading.Thread(target=run_jpeg_compression).start()
    def run_compressor(self, args):
        compressor_path = os.path.join(self.application_path, "compressor.exe")
        if not os.path.exists(compressor_path):
            raise FileNotFoundError(f"Compressor executable not found at: {compressor_path}")

        try:
            process = subprocess.Popen(
                args,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            stdout, stderr = process.communicate()

            if process.returncode != 0:
                raise RuntimeError(f"Compression failed: {stderr}")

            return stdout
        except Exception as e:
            raise Exception(f"Error running compressor: {str(e)}")
    def run(self):
        self.root.mainloop()

if __name__ == "__main__":
    app = CompressorGUI()
    app.run()