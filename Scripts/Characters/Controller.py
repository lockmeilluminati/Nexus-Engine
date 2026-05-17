# Nexus Engine - Default Controller
import nexus

def update(character):
    # Hardcoded controls are now dead. You have full command over physics!
    # Try modifying these values live while the engine is running.
    
    if character.is_grounded:
        pass # Custom idle routines can sit here
