import glob
import os
import sys
import time
#import man

try:
    sys.path.append(glob.glob('../carla/dist/carla-*%d.%d-%s.egg' % (
        sys.version_info.major,
        sys.version_info.minor,
        'win-amd64' if os.name == 'nt' else 'linux-x86_64'))[0])
except IndexError:
    pass

import carla

import argparse
import logging
import random


def main():
    argparser = argparse.ArgumentParser(
        description=__doc__)
    argparser.add_argument(
        '--host',
        metavar='H',
        default='127.0.0.1',
        help='IP of the host server (default: 127.0.0.1)')
    argparser.add_argument(
        '-p', '--port',
        metavar='P',
        default=2000,
        type=int,
        help='TCP port to listen to (default: 2000)')
    args = argparser.parse_args()

    logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)

    client = carla.Client(args.host, args.port)
    SetAutopilot = carla.command.SetAutopilot
   
    client.set_timeout(10.0)
    ambulance = None

    try:

        world = client.get_world()
        ambulance = None
        # Can be extended to take in a generic valid blueprint
        ambulance_bp = world.get_blueprint_library().find('vehicle.ford.ambulance')
        ambulance_bp.set_attribute('role_name','autopilot')
        print('\Ambulance role_name is set')
        
        if ambulance_bp.has_attribute('color'):
            ambulance_color = random.choice(ambulance_bp.get_attribute('color').recommended_values)
            ambulance_bp.set_attribute('color',ambulance_color)
        if ambulance_bp.has_attribute('driver_id'):
            driver_id = random.choice(ambulance_bp.get_attribute('driver_id').recommended_values)
            ambulance_bp.set_attribute('driver_id', driver_id)
        else:
            ambulance_bp.set_attribute('role_name', 'autopilot')
        print('\nColor is set')
        #ambulance_bp.set_autopilot(enabled=True,port=8000)
        #SetAutopilot(ambulance_bp, True, port=8000)
        batch = []
        # Set Autopilot
        
        spawn_points = world.get_map().get_spawn_points()

        # Would it be better for it to be consistent rather than random
        if len(spawn_points) > 0:
            random.shuffle(spawn_points)
            transform = spawn_points[0]
            ambulance = world.spawn_actor(ambulance_bp,transform)
            
            #if args.autopilot:
            #vehicle.apply_control(carla.VehicleControl(throttle=0.1, brake=0.1))
            batch.append(SetAutopilot(ambulance, True))
            client.apply_batch_sync(batch)
            print('\nAmbulance is spawned')
        else: 
            logging.warning('Could not found any spawn points')

        # --------------
        # Game loop. Prevents the script from finishing.
        # --------------
        while True:
            world_snapshot = world.wait_for_tick()

    finally:
        if ambulance is not None:
            ambulance.destroy()

if __name__ == '__main__':

    try:
        main()
    except KeyboardInterrupt:
        pass
    finally:
        print('\nDone with ambulance.')