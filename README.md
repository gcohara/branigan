## Status

Usable, but requires further testing.

# Branigan

A GNU/Linux program for restricting unproductive internet usage.

## Purpose

Branigan was originally concieved as a port of the excellent [**SelfControl**](https://selfcontrolapp.com/) for macOS, and shares its objective of blocking access to websites you find distracting until a given end time.  

Note that Branigan will only work on distributions using **systemd**. Fortunately, barring a few enthusiast distros, this is most of them.

## Installation

Currently, installing Branigan requires **CMake** and a **C++ compiler** to be installed.  

First, clone the repository:  
```
git clone https://github.com/gcohara/branigan.git 
```

Then navigate into the Branigan folder, and create the build and binary folders:
```
mkdir build && mkdir bin
```

Now navigate to the build folder, run CMake, and run make:  
```
cd build/ && cmake .. && make
```

Finally (and optionally) create a symbolic link to the Branigan binary:  
```
sudo ln -s $(pwd)/../bin/branigan /usr/local/bin/branigan
```

## Usage

Branigan has a command line interface only, and needs to be run with `sudo`. This was chosen deliberately to avoid the complications of Linux GUIs. Also, the average GNU/Linux user tends to be more confident with using a terminal.  

The simplest way to use Branigan is first by creating a blocklist in the branigan directory (a default sample blocklist is included):  
```
touch branigan-blocklist
```

Then add your sites to this blocklist, for example:  
```
thetimes.co.uk  
news.ycombinator.com  
reddit.com  
old.reddit.com
```


Now you can start a block until 5pm as follows:  
```
sudo branigan --install 17:00
```  
If it's before 5pm, Branigan will assume that you mean 5pm today. Otherwise, it will assume you mean 5pm tomorrow, and install a block that lasts until then. 

If you want a block that lasts longer than 24 hours, you can provide a datetime in ISO8601 form, e.g 2020-01-31T17:00 for 5pm on the 31st January 2020:  
```
sudo branigan --install 2020-01-31T17:00
```  

Branigan can also be started using a different blocklist to the default one (`.braniganblocklist`):  
```
sudo branigan --install ~/.some-other-blocklist 17:00
```  

If you think Branigan should have cleared a block but hasn't, try running:  
```
sudo branigan --checkup
```  

This will check if the block has ended, and clear it if so.

## Alternatives

There are two similar programs for Linux that I'm aware of:  
- [**Chomper:**](https://github.com/aniketpanjwani/chomper) This takes a different approach to how its blocking works, and looks good. Unfortunately, it hasn't been updated in a while (as of July 2020).
- [**Self Control Linux:**](https://github.com/zengargoyle/SelfControl) This was a more faithful port of the original SelfControl, but is now unfortunately deprecated.

# TODO
### README
- Explanation of name
### CODE
- Automated blocking
- Add install script
