import pygame
import serial
import numpy as np
from pygame.locals import *

ARM_LENGTH = 160
SENSITIVITY = 1850
SCREEN_HEIGHT = 800
SCREEN_WIDTH = 1600

ARM_THICKNESS = 4
COORD_THICKNESS = 4
MAGENTA = (255, 0, 255)
CYAN = (0, 255, 255)
GREEN = (50, 255, 50)
LIGHT_GREEN = (150, 255, 150)
RED = (255, 50, 50)
GRAY = (150, 150, 150)
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)

BUTTON_TOP = 40
BUTTON_MOST_LEFT = SCREEN_WIDTH//3
BUTTON_HEIGHT = 80
BUTTON_WIDTH = 160
BUTTON_SPACING = 40

last_ticks = None

elbow = (160, 0, 0)
forarm = (0, 0, 160)
    
def draw_coords(screen):
    white = (255, 255, 255)
    length = 200
    space = 20
    # Birds perspective 
    pygame.draw.line(screen, white, (SCREEN_WIDTH//3 - length, SCREEN_HEIGHT//2 + length), (SCREEN_WIDTH//3 + length, SCREEN_HEIGHT//2 + length), width = COORD_THICKNESS)
    pygame.draw.line(screen, white, (SCREEN_WIDTH//3 - length, SCREEN_HEIGHT//2 + length), (SCREEN_WIDTH//3 - length, SCREEN_HEIGHT//2 - length), width = COORD_THICKNESS)
    font = pygame.font.SysFont(None, 36)
    img = font.render('Bird view', True, white)
    screen.blit(img, (SCREEN_WIDTH//3 - length//2, SCREEN_HEIGHT//2 + length + space))
    img = font.render('Y', True, white)
    screen.blit(img, (SCREEN_WIDTH//3 + length, SCREEN_HEIGHT//2 + length + space))
    img = font.render('X', True, white)
    screen.blit(img, (SCREEN_WIDTH//3 - length - space, SCREEN_HEIGHT//2 - length))
    img = font.render('Z', True, white)
    screen.blit(img, (SCREEN_WIDTH//3 - length - space, SCREEN_HEIGHT//2 + length))


    pygame.draw.line(screen, white, (2*SCREEN_WIDTH//3 - length, SCREEN_HEIGHT//2 + length), (2*SCREEN_WIDTH//3 + length, SCREEN_HEIGHT//2 + length), width = COORD_THICKNESS)
    pygame.draw.line(screen, white, (2*SCREEN_WIDTH//3 - length, SCREEN_HEIGHT//2 + length), (2*SCREEN_WIDTH//3 - length, SCREEN_HEIGHT//2 - length), width = COORD_THICKNESS)
    # font = pygame.font.SysFont(None, 36)
    img = font.render('View from right', True, white)
    screen.blit(img, (2*SCREEN_WIDTH//3 - length//2, SCREEN_HEIGHT//2 + length + space))
    img = font.render('X', True, white)
    screen.blit(img, (2*SCREEN_WIDTH//3 + length, SCREEN_HEIGHT//2 + length + space))
    img = font.render('Z', True, white)
    screen.blit(img, (2*SCREEN_WIDTH//3 - length - space, SCREEN_HEIGHT//2 - length))
    img = font.render('Y', True, white)
    screen.blit(img, (2*SCREEN_WIDTH//3 - length - space, SCREEN_HEIGHT//2 + length))
    
def draw_arm_birdview(screen, elbow, hand):
    pygame.draw.line(screen, MAGENTA, (SCREEN_WIDTH//3, SCREEN_HEIGHT//2), (SCREEN_WIDTH//3 + elbow[1], SCREEN_HEIGHT//2 - elbow[0]), width=ARM_THICKNESS)
    pygame.draw.line(screen, CYAN, (SCREEN_WIDTH//3 + elbow[1], SCREEN_HEIGHT//2 - elbow[0]), (SCREEN_WIDTH//3 + elbow[1] + hand[1], SCREEN_HEIGHT//2 - elbow[0] - hand[0]), width=ARM_THICKNESS )

def draw_arm_from_right(screen, elbow, hand):
    pygame.draw.line(screen, MAGENTA, (2*SCREEN_WIDTH//3, SCREEN_HEIGHT//2), (2*SCREEN_WIDTH//3 + elbow[0], SCREEN_HEIGHT//2 - elbow[2]), width=ARM_THICKNESS)
    pygame.draw.line(screen, CYAN, (2*SCREEN_WIDTH//3 + elbow[0], SCREEN_HEIGHT//2 - elbow[2]), (2*SCREEN_WIDTH//3 + elbow[0] + hand[0], SCREEN_HEIGHT//2 - elbow[2] - hand[2]), width=ARM_THICKNESS)

def draw_buttons(screen, pressed_button:int = None):
    punish_color = MAGENTA if pressed_button == 0 else RED
    reward_color = LIGHT_GREEN if pressed_button == 1 else GREEN
    calibrate_color = WHITE if pressed_button == 2 else GRAY
    font = pygame.font.SysFont(None, 36)
    def draw_button(screen, color:tuple, action:str, left:int, top:int):
        rect = pygame.Rect(left, top, BUTTON_WIDTH, BUTTON_HEIGHT)
        pygame.draw.rect(screen, color, rect)
        img = font.render(action, True,  BLACK)
        
        screen.blit(img, (left + BUTTON_WIDTH//2 - img.get_width()//2, top + BUTTON_HEIGHT//2 - img.get_height()//2))


    draw_button(screen, punish_color, "Punish", BUTTON_MOST_LEFT, BUTTON_TOP)
    draw_button(screen, reward_color, "Reward", BUTTON_MOST_LEFT + BUTTON_WIDTH + BUTTON_SPACING, BUTTON_TOP)
    draw_button(screen, calibrate_color, "Calibrate", BUTTON_MOST_LEFT + 2*BUTTON_WIDTH + 2*BUTTON_SPACING, BUTTON_TOP)

def handle_event(button_pressed:int):
    global Serial
    global last_ticks
    last_ticks = pygame.time.get_ticks()
    if Serial is not None:
        if button_pressed == 0:
            Serial.write(b"P")
        elif button_pressed == 1:
            Serial.write(b"R")
        elif button_pressed == 2:
            Serial.write(b"C")
    else:
        print(button_pressed)

def read_data():
    global Serial
    global forarm
    global elbow
    line = Serial.readline().decode()
    if "Problem" in line:
        return 
    try:
        line = list(line.split(" "))
        data = np.array(list(map(lambda x: float(x), line[1:])))*ARM_LENGTH
        if str(line[0]) == "F":
            forarm = data
        elif str(line[0]) == "A":
            elbow = data
        else:
            raise NotImplementedError
    except Exception as e:
        print(e)







def main():
    global Serial
    global last_ticks
    global elbow
    global forarm
    try:
        Serial = serial.Serial('/dev/tty.usbmodem1101', 115200, timeout=1)
    except Exception as e:
        print(e)
        Serial = None

    
    pygame.init()
    screen = pygame.display.set_mode((SCREEN_WIDTH,SCREEN_HEIGHT))
    pygame.display.set_caption("Press Esc to quit")
    frames = 0
    ticks = pygame.time.get_ticks()
    button_pressed = None
    last_ticks = None
    while 1:
        ticks =  pygame.time.get_ticks()
        event = pygame.event.poll()
        if event.type == QUIT or (event.type == KEYDOWN and event.key == K_ESCAPE):
            pygame.quit()  #* quit pygame properly
            break  
            
        # button_pressed = None
        if event.type == KEYDOWN:
            if event.key == K_p:
                # punish()
                button_pressed = 0
                handle_event(button_pressed)
            elif event.key == K_r:
                # reward()
                button_pressed = 1
                handle_event(button_pressed)
            elif event.key == K_c:
                # calibrate()
                button_pressed = 2
                handle_event(button_pressed)
        if event.type == MOUSEBUTTONDOWN:
            mouse = pygame.mouse.get_pos()
            if BUTTON_TOP <= mouse[1] <= BUTTON_TOP + BUTTON_HEIGHT:
                if BUTTON_MOST_LEFT <= mouse[0] <= BUTTON_MOST_LEFT + BUTTON_WIDTH:
                    button_pressed = 0
                    handle_event(button_pressed)
                elif BUTTON_MOST_LEFT + BUTTON_WIDTH + BUTTON_SPACING <= mouse[0] <= BUTTON_MOST_LEFT + BUTTON_SPACING + 2* BUTTON_WIDTH:
                    button_pressed = 1
                    handle_event(button_pressed)
                elif BUTTON_MOST_LEFT + 2*BUTTON_WIDTH + 2*BUTTON_SPACING <= mouse[0] <= BUTTON_MOST_LEFT + 2*BUTTON_SPACING + 3*BUTTON_WIDTH:
                    button_pressed = 2
                    handle_event(button_pressed)

        if button_pressed is not None and (ticks - last_ticks > 200):
            button_pressed = None

        if Serial is not None:
            read_data()


        # elbow, hand = process_data(data[0], data[1])
        screen.fill((0,0,0))
        draw_coords(screen)
        draw_arm_birdview(screen, elbow, forarm)
        draw_arm_from_right(screen, elbow, forarm)
        draw_buttons(screen, button_pressed)
        pygame.display.flip()   
        
      
        # time.sleep(1)
        frames = frames+1


    # print ("fps:  %d" % ((frames*1000)/(pygame.time.get_ticks()-ticks)))
    Serial.close()

if __name__ == '__main__': main()