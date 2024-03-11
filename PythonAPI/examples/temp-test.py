import carla
from pathlib import Path

counter = 0

try:
  client = carla.Client('localhost', 2000)
  client.set_timeout(2)
  world = client.get_world()
  bpl = world.get_blueprint_library()
  transform = world.get_map().get_spawn_points()[0]
  camera_bp = bpl.find('sensor.camera.rgb')
  camera = world.spawn_actor(camera_bp, transform)
  camera.listen(lambda im:
    im.save_to_disk(
      Path(__file__).parent / f'_out/{counter}.png',
      carla.ColorConverter.Raw))
  while True:
    world.tick()
except Exception as e:
  print(f'Exception thrown {e}')
finally:
  camera.destroy()