# cat_in_a_box
Simulating a cat in a box for a white elephant gift exchange.

# What I need to build
Ultimately this shoud do it's best to simulate a cat stuck in a box.  The arduino should detect being picked up, tipped, shaken, etc and respond accordingly.
- It should have a variety of cat noises that are randomly produced as it sits on a table.
- It needs to run in very low power as it'll have to run off a battery, so 5V might be the max that it can use
- If possible, a motor should be used to create physical noises as though the cat is moving, scratching, etc.
- Use a speaker and mp3 converter to produce noise
- it'll probably need to accept an SD card with cat noises
- Accelerometer or tip sensor to detect movement?
- Needs to physically hold up to scrutiny, meaning it needs to be constructed in a way that can handle being shaken

# Start by detecting movement and responding accordingly
- Detect movement
- Detect VIOLENT movement
- output text differentiating the two
- What options do I have?
    - DO I have an acceleramoter?
    - Learn the tilt ball switch


reaction:
safe state    = (< minor shake & flat) - a little random movement and very rare and random meowing
being an ass  = (< minor shake & !flat) | minor shake - some movement and repeated meowing until it's badk to a safe state
minor shake   = (> 1.4 on threshold scale) - a less random movement and less rare and random meowing
mild shake    = (> 2.0 on threshold scale) - repeated minor panic and hissing
violent shake = (> 2.5 on threshold scale) - repeated hissing, panic, screaming until it's badk to a safe state


baseline 
-0.001	0.003	0.000	0.006	0.000	0.258	Flat


-0.011	0.009	
0.000	0.022	
0.000	1.025	
Flat

x -0.022	0.000	
y -0.989	0.000	
z -0.047	0.000

mild movement
-0.147	0.167	-0.169	0.144	-0.161	0.509	Flat


-0.804	0.563	
-0.316	0.425	
0.000	1.999	
Flat

x -0.452	0.460	
y -1.786	0.000	
z -0.630	0.518

violent movement
x -2.000	1.689	
y -2.000	1.625	
z -1.380	1.267




Averages:

baseline
0.01

walking
0.4

mild shake
0.56

violent shake
0.8