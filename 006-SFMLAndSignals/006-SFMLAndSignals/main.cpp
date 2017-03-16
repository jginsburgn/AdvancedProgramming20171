
//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// In order to load the resources like cute_image.png, you have to set up
// your target scheme:
//
// - Select "Edit Scheme…" in the "Product" menu;
// - Check the box "use custom working directory";
// - Fill the text field with the folder path containing your resources;
//        (e.g. your project folder)
// - Click OK.
//

// *-*-*-*-* INCLUDES

//C libraries
#include <signal.h>
#include <unistd.h>
#include <math.h>

//C++ libraries
#include <iostream>

//SFML libraries
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

// INCLUDES *-*-*-*-*

// *-*-*-*-* PREPROCESSOR DEFINITIONS AND GLOBAL VARIABLES

//Window properties
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600

//Circle properties
#define CIRCLE_RADIUS_SPEED 30 //Given in pixels per second
#define CIRCLE_BORDER_THICKNESS 1
#define CIRCLE_RADIUS_INITIAL 0
#define CIRCLE_POINT_COUNT 50
#define CIRCLE_BORDER_COLOR sf::Color::Black

//Animation properties and variables
#define ANIMATION_HALF_PERIOD (float)3
bool animationStopped = false;

//Directions and variable
#define POSITIVE 1
#define NEGATIVE -1
int direction = POSITIVE;

//Last elapsed time
long long lastElapsedTime = 0; //This variable is used to calculate the time elapsed in a single frame rendering and account for the accumulated time elapsed

//Last alarm set time
long long lastAlarmSetTime = 0; //This variable tells when was the last alarm timer started

long long alarmIntervalActuallyAnimated = 0; //This variable accounts for the amount of time in a given timer() interval (i.e. from the time the alarm was set to when it goes off) in which the animation was going, i.e. animationStopped was false

// PREPROCESSOR DEFINITIONS AND GLOBAL VARIABLES *-*-*-*-*

// *-*-*-*-* FUNCTION DECLARATIONS

//Get seconds from microseconds
float seconds(const long long microseconds);

//Center circle in window
void centerCircle(sf::CircleShape * circle);

//Resize circle given a radius change
void resizeCircle(sf::CircleShape * circle, const float radiusChange);

//Calculate the absolute radius change given the time change
float absoluteRadiusChange(const float timeChangeInSeconds);

//Stop the animation when SIGINT is received
void SIGINTHandler(int);

//Resume the animation when SIGTSTP is received
void SIGTSTPHandler(int);

//Change direction of the animation when SIGALRM is received
void SIGALRMHandler(int);

// FUNCTION DECLARATIONS *-*-*-*-*

// *-*-*-*-* FUNCTION IMPLEMENTATIONS

int main(int argc, char const** argv) {
    //Clean the screen for better looking program
    system("clear");
    
    //Show welcome message
    printf("Please be sure to run the program with a working directory equal to the path where the provided resources (icon.png and sensation.ttf) are found.\n\nWelcome to the SFMLAndSignals v0.0.0 program by Jonathan Ginsburg (A01021617). It (v0.0.0) was finished on the morning (2:00) of March 16, 2017 for the Advanced Programming class. It uses the SFML library to animate a simple circle and signals to control such animation. System calls used include: alarm(), sigaction(), sigdelset(), sigfillset(), and sigprocmask().\n\nPID: %i.\n\nEnter CTRL + Z (SIGINT), to stop the animation or CTRL + Z (SIGTSTP) to resume the animation if stopped.", getpid());
    fflush(stdout);
    
    //Arrange signal handler details for SIGINT, i.e. CTRL + C
    struct sigaction SIGINTAction;
    SIGINTAction.sa_handler = SIGINTHandler;
    sigemptyset(&SIGINTAction.sa_mask);
    SIGINTAction.sa_flags = 0;
    //Register signal handler
    sigaction(SIGINT, &SIGINTAction, NULL);
    
    //Arrange signal handler details for SIGTSTP, i.e. CTRL + Z
    struct sigaction SIGTSTPAction;
    SIGTSTPAction.sa_handler = SIGTSTPHandler;
    sigemptyset(&SIGTSTPAction.sa_mask);
    SIGTSTPAction.sa_flags = 0;
    //Register signal handler
    sigaction(SIGTSTP, &SIGTSTPAction, NULL);
    
    //Arrange signal handler details for SIGALRM, i.e. signal when alarm()'s timer stops
    struct sigaction SIGALRMAction;
    SIGALRMAction.sa_handler = SIGALRMHandler;
    sigemptyset(&SIGALRMAction.sa_mask);
    SIGALRMAction.sa_flags = 0;
    //Register signal handler
    sigaction(SIGALRM, &SIGALRMAction, NULL);
    
    //Ignore all other unused signals
    //Prepare mask set
    sigset_t set;
    sigfillset(&set);
    sigdelset(&set, SIGINT);
    sigdelset(&set, SIGTSTP);
    sigdelset(&set, SIGALRM);
    //Install mask set
    sigprocmask(SIG_SETMASK, &set, NULL);
    
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "006-SFMLAndSignals");

    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile("icon.png")) {
        return EXIT_FAILURE;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Create a graphical text to display
    sf::Font font;
    if (!font.loadFromFile("sansation.ttf")) {
        return EXIT_FAILURE;
    }
    sf::Text text("", font, 12);
    text.setFillColor(sf::Color::Black);
    
    //Circle
    sf::CircleShape circle(CIRCLE_RADIUS_INITIAL, CIRCLE_POINT_COUNT);
    circle.setOutlineColor(CIRCLE_BORDER_COLOR);
    circle.setOutlineThickness(CIRCLE_BORDER_THICKNESS);
    centerCircle(&circle);
    
    //Clock for measuring elapsed time
    sf::Clock clock;

    //Set an alarm (no need to update lastAlarmSetTime because elapsed time to this point is negligible to alarm()'s argument lack of precision, which is given in seconds)
    alarm(ANIMATION_HALF_PERIOD);
    
    // Start the game loop
    while (window.isOpen())
    {
        //Calculate needed time differences and accumulations
        long long aid = clock.getElapsedTime().asMicroseconds(); //Aid variable holds the total elapsed time at the beginning of the rendering
        long long timeDifference = aid - lastElapsedTime; //Time difference holds the elapsed time in the last single frame rendering
        lastElapsedTime = aid; //Accumulate the total elapsed time at the beginning of the rendering for next frame calculations
        
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        
        //Display the accumulated elapsed time in seconds
        text.setString(std::to_string(seconds(lastElapsedTime)));

        // Clear screen
        window.clear(sf::Color::White);

        // Draw the string
        window.draw(text);
        
        //Draw the circle
        resizeCircle(&circle, direction * absoluteRadiusChange(seconds(timeDifference)));
        centerCircle(&circle);
        window.draw(circle);

        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}

//Get seconds from microseconds
float seconds(const long long microseconds) {
    return microseconds * (float)0.000001;
}

//Center circle in window
void centerCircle(sf::CircleShape * circle) {
    circle->setPosition(WINDOW_WIDTH/2 - circle->getRadius(), WINDOW_HEIGHT/2 - circle->getRadius());
}

//Resize circle given a radius change
void resizeCircle(sf::CircleShape * circle, const float radiusChange) {
    circle->setRadius(circle->getRadius() + radiusChange);
}

//Calculate the absolute radius change given the time change
float absoluteRadiusChange(const float timeChangeInSeconds) {
    if (animationStopped) {
        return 0;
    }
    return CIRCLE_RADIUS_SPEED * timeChangeInSeconds; //∆s = ∆t · v, radius change equal to time change times speed
}

//Stop the animation when SIGINT is received
void SIGINTHandler(int) {
    if (!animationStopped) {
        animationStopped = true;
        alarmIntervalActuallyAnimated = lastElapsedTime - lastAlarmSetTime;
    }
}

//Resume the animation when SIGTSTP is received
void SIGTSTPHandler(int) {
    if (animationStopped) {
        animationStopped = false;
        int alarmSeconds = (int)round((ANIMATION_HALF_PERIOD - seconds(alarmIntervalActuallyAnimated)));
        if (alarmSeconds == 0) {
            SIGALRMHandler(0);
            return;
        }
        alarm(alarmSeconds);
        lastAlarmSetTime = lastElapsedTime;
    }
}

//Change the direction of the animation when SIGALRM is recieved
void SIGALRMHandler(int) {
    if (!animationStopped) {
        direction *= NEGATIVE;
        alarm(ANIMATION_HALF_PERIOD);
        lastAlarmSetTime = lastElapsedTime;
    }
}

// FUNCTION IMPLEMENTATIONS *-*-*-*-*
